/*watergaugeV2_HC10_Sk.ino   SakuraA3基板 10m 温度補正有り GPS Sakura */
#define VernNo "Ver3.2"
/* スリープ中の雨量計検知時には一旦スリープを解除し各センサ出力を測定しサーバへ送信し再びスリープ*/
//制御フラグ
boolean Initialization=1;  //1:Initialization

//WDTタイムアウト
#include <avr/wdt.h>
volatile byte wdt_cycle = 0;           // WDTタイムアウトカウント
#define timeout_count10 72           //10分(72) WDTソフトリセット時間設定 RTC無し前提
//#define timeout_count5 16             //test用 8秒 WDTソフト割り込み時間設定
int timeout_adjust = 0;
//SleepMode
#include <avr/sleep.h>
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//Arduinoライブラリ
#include <Wire.h>

//さくらのIoT
#include <SakuraIO.h>

#define PIN_RAIN 2                   //PIN_RAINをデジタルI/Oピン2に設定

#define Sakura 3 //さくらのIoTの電源ON/OFF HIGHでON
#define water10Power 9  //10m用超音波センサ電源（温度センサMCP9701-E電源）
#define max_range 1023 //超音波センサの最大測定距離cm
#define Water10_const 2     //2cm/1024
#define TRON_GPS 10    //PVの8.2Ω短絡による電流測定ON（HIGH）GPS電源ON（HIGH）と兼用

/*変数定義*/
unsigned long Time_n=0;  //RTCレス時の計測カウンタ
volatile byte numClicksRain = 0;      //雨量計のカウント数を初期化
volatile boolean RainAction = 0;         //雨量計割込フラグ
int Rainfall = 0;             //降雨量を初期化(10倍値をintで保持)
byte RtcCount = 0;                    //RTC動作カウント用
float Eneloop = 5.88;                //電池電圧
#define Eneloop_const 0.0064453125      //Eneloop電池電圧AD値からの換算係数 
#define Radiation_constant 1.34     //2Wパネル日射量センサ定数 Fan有り1.41　Fan無し1.34
byte NoRadiationCount=0;          //日射量が0が続くカウント

int Distance_send=max_range;      //前回送信データ_水位距離（センサ端面）mm
int Distance_ave=max_range;
byte invalid_Clong=0;   //最大距離オーバー無効距離測定回数
#define measurec 7     //測定回数
SakuraIO_I2C sakuraio;
unsigned long Measuretime=0;   //測定終了時の時間保持
unsigned long sendDelay=0;      //測定終了からデータを送信完了するまでの遅延時間

float temperature = 0;               //気温を初期化
#define temp_gain 0.1653     // ℃・LSB =3.3/1024*1000/19.5
#define temp_offset  124   // 0℃ 0.4V  0.4/3.3*1024=124
boolean send_Flg=0;         //送信フラグ送信時1
boolean night=1;          //日射量が0ならnight=1
boolean GPSflag=0;        //GPSflag GPS測定失敗ならfalse (0)
unsigned long latn=0;      //北緯データ
unsigned long lnge=0;      //東経データ

void setup() {  //  Will be called only once.
  pinMode(TRON_GPS,OUTPUT);   
  digitalWrite(TRON_GPS,LOW);    //TR_OFF,GPS＿OFF
  pinMode(Sakura,INPUT);
  WDT_setup8();                          // 8秒のWDT設定
  // 日射量を検知したらスタート
  int val3=0;
  while(val3==0){
    digitalWrite(TRON_GPS,HIGH);         //TR ON  2Wパネル日射量測定の為太陽電池出力短絡ON
    delay(50);
    val3=ReadSens_ch(3,1,50);      //2Wパネル日射量AD3の5回平均値(個別ch, 読取回数, intarvalms)
    digitalWrite(TRON_GPS,LOW);          //TR OFF  太陽電池出力短絡OFF
    if(val3>0) break;
    system_sleep();         //delayの代わりに8秒sleepを利用
    wdt_cycle=0;            //softreset防止の為カウンタを毎回初期化
  }
  Serial.begin(9600);
  pinMode(PIN_RAIN, INPUT_PULLUP);                         //PIN_RAIN(デジタルI/Oピン2)を入力ピンに設定
  attachInterrupt(0, countRain, FALLING);           //割り込み番号1:FALLINGピンの状態がHIGHからLOWに変わったときにcountRain関数を呼び出し
//  Wire.begin();
  Serial.println("---Start Loop---");
  Serial.end();    // Txの漏れ電流対策
}

void loop()
{
  Time_n++;
  digitalWrite(TRON_GPS,HIGH); //TR ON  2Wパネル日射量測定の為太陽電池出力短絡ON　GPS ON兼用
//  delay(50);
  Eneloop=Eneloop_const*ReadSens_ch(1,2,50);      //Eneloop電圧AD1の4回平均値(個別ch, 読取回数, intarvalms)(delay100も兼ねている)
  int Radiation=Radiation_constant*ReadSens_ch(3,4,50); //日射量:2Wパネル日射量AD3の5回平均値(個別ch, 読取回数, intarvalms)
  if (Initialization == 1) {   //Arduino起動後に雨量計が1カウント誤検知するため、ループ1回目は強制的に雨量計・風速計のカウント数を0にする。
    numClicksRain = 0;
  }else   digitalWrite(TRON_GPS,LOW);  //TR OFF  太陽電池出力短絡OFF　GPS OFF 初回のみOFFしない。
  if( Eneloop>4.6 && RtcCount%6==0){ //RtcCount==0ならここでsakura.io 電源ONしておく
    pinMode(Sakura,OUTPUT);
    digitalWrite(Sakura,HIGH);       //GPSONのタイミングでSakura電源もONしておく
    if( Radiation==0 ) NoRadiationCount++; //日射量が0ならNoRadiationCountをインクリメント
    else NoRadiationCount=0;               //日射量があればNoRadiationCountをクリア
  }

  int DistA[measurec];           //4秒間隔で7回測定値保存用
  for(int m=0; m<measurec; m++){   //4秒間隔で7回測定
    Measuretime = millis(); //測定終了時刻を保存する
    DistA[m]=WaterGauge10_ave(7,100,20);//水位の有効測定値n回の内真ん中の値(有効値計測回数n，間隔ms,上限回数)
    //長い距離が測定されてしまう誤動作対策
    if( abs(Distance_ave - DistA[m]) >=4 || abs(Distance_send - DistA[m]) >=5 || DistA[m]==max_range){  //平均値と中心値が4cm,前回送信値より5cm以上差があったら再測定
      delay(2000);   //再測定前に2秒待ち
      int Dist_temp = WaterGauge10_ave(7,100,20);//水位の有効測定値n回の内の真ん中の値(有効値計測回数n，間隔ms,上限回数)
      if( abs(Distance_send - Dist_temp) < abs(Distance_send - DistA[m]) ) DistA[m] = Dist_temp;  //2回測って差が小さい方を設定 
    } 
//    Serial.print("Distance[]=");
//    Serial.println(DistA[m]);
    if(m<measurec-1) {
      for(byte t;t<40;t++){       //最大0.1x40=4秒
        if(millis()-Measuretime>4000)break;
        delay(100);
      }
    }
  }
  int Distance = long_ave(DistA);    //最大最小を除いた平均値 max_rangeの値も除く
//  Serial.println(Distance);
  
  // 前回送信データよい5cm短いかmax_rangより短く前回送信データより8cm長いと送信(1時間カウンタをリセット)
  if (Distance <= Distance_send-5 || (Distance >= Distance_send+8 && Distance < max_range))  send_Flg=1;
 
  //電池電圧>4.6かつsend_Flg=1 なら送信
  if( Eneloop>4.6 && (send_Flg==1 || RtcCount%6==0) ){ //sakura.io 電源ONしてデータ送信
    
    //GPSを1日2回測定、日の出と日の入りのタイミング。
    if( (Radiation > 0 && night==1) || GPSflag==false || (Radiation == 0 && night==0) ){
      GPSflag = GPS_get();//GPS取得,夜から日射量が出始めたら測定、前回失敗なら測定
      Serial.end();    // Txの漏れ電流対策
    }
    if(Radiation > 0) night=0;   //日射量があればnightフラグは0、無ければ1（true）
    else night=1;

    pinMode(Sakura,OUTPUT);
    digitalWrite(Sakura,HIGH);       //GPSONのタイミングでSakura電源もONしておく

      boolean connectflag = connectLTE();//LTE接続を試行

      if (connectflag==1) {      //LTE接続ならデータを送信
        sakuraio.enqueueTx(2,(float)Distance,sendDelay); //Distance ID=2で送信
        sakuraio.enqueueTx(7,(float)Radiation,sendDelay); //Radiation
        sakuraio.enqueueTx(1,temperature, sendDelay); //temperature ID=1で送信
        sakuraio.enqueueTx(11,(float)Rainfall*0.1,sendDelay); //Rainfall(*0.1) ID=11で送信
        sakuraio.enqueueTx(15,Eneloop,sendDelay);      //電池電圧を送信
        if(GPSflag==true){    //  GPS取得成功ならGPSデータを送信  
          sakuraio.enqueueTx(12,(float)latn/1000000,sendDelay); //latn ID=12で送信
          sakuraio.enqueueTx(13,(float)lnge/1000000+100,sendDelay); //lnge ID=13で送信
        }
        sakuraio.send();    //Datastoreに送信開始

        uint8_t queue,immediate;
        for(;;){
          sakuraio.getTxStatus(&queue, &immediate); //getTxStatus(uint8_t *queue, uint8_t *immediate);
          if( queue==0 ) break;
          delay(100);
        }
        uint32_t unixtime_now = 0;
        for(int i=0; i<5; i++){ //2019/1/10追加 たまに取得失敗するので5秒試行する
          unixtime_now = (uint32_t)(sakuraio.getUnixtime()/1000UL);
          if( unixtime_now != 0 ) break;
          delay(1000);
        }
        if(unixtime_now != 0) {
          byte wdt_old=wdt_cycle;
          wdt_cycle=(int)((unixtime_now%600)/8.192+0.5);//次の10分までのためのWDTを設定RTCレスの場合+2を外す
          RtcCount=(int)((unixtime_now%3600)/600);//データ送信後にRtcCountをセット
          if( unixtime_now%600 >=  510 ){
             wdt_cycle=0; //1分30秒以内ならwdt_cycleをクリアして1分以内の再送信をしない。
             RtcCount=6;  //1分30秒以内ならRtcCountを6にセット
          }
          if( wdt_old >= wdt_cycle + 3  ) timeout_adjust++;
          else if( wdt_old + 3 <= wdt_cycle ) timeout_adjust--;
        }
        send_Flg=1;
        if (Distance < max_range ) Distance_send=Distance; //Distance_sendを更新
      }else  send_Flg=0;    //未送信

  }

//Eneloop電池電圧が5.9V以上で日射量があるとき、電流測定回路をONにして過充電を防止する。
  Eneloop=Eneloop_const*ReadSens_ch(1,4,50);      //Eneloop電圧AD1の4回平均値(個別ch, 読取回数, intarvalms)(delay200も兼ねている)
  if (Eneloop>6.0 ){
    digitalWrite(TRON_GPS,HIGH);//TR ON  電流測定回路ON(GPS電源と兼用) 
  }

  pinMode(Sakura,INPUT);   //ポートをINPUTに戻しておく

  if( NoRadiationCount > 24)  software_Reset(); //日射量が24時間連続で0ならリセットし、setup内で日射量を検知するまでスリープ
  
//sleepからの復帰が雨量計の場合およびWDTの場合は再度スリープに戻る。
  do{
    RainAction = 0; //スリープ後のスリープ復帰判断用にkを初期化  //anan Sleep復帰から計測～送信完了までの間にRainAction = 1になった場合の対応
    //Sleep mode Setup
    system_sleep();
  }while(RainAction !=0 || wdt_cycle < timeout_count10 + timeout_adjust);  // timeout_count10(x8.192秒)以上経過したら抜ける
  wdt_cycle=0; //WDTのカウントのリセット
  RtcCount++; //RTCからの割り込み回数のカウント
  if (RtcCount > 6){       //6×10分(1時間)経過したら初期化
    Rainfall = (int)(10*(0.2794 * numClicksRain)+0.5);//60分に計測された雨量計のカウント数より1時間当たりの降水量を算出_60分保持,10倍してint保持
    numClicksRain = 0;
    RtcCount = 1;      
  }
  //Initialization
  Initialization = 0;			//Initializationフラグを0にする
  send_Flg = 0;                         //送信フラグをクリア
  sendDelay = 0;
}

void system_sleep() {                   // システム停止
  cbi(ADCSRA, ADEN);                    // ADC 回路電源をOFF (ADC使って無くても120μA消費するため）
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // パワーダウンモード
  sleep_enable();
  sleep_mode();                         // ここでスリープに入る
  sleep_disable();       // RTCからの割り込みが発生したらここから動作再開
  sbi(ADCSRA, ADEN);     // ADC ON
}

//スイッチ割り込み処理（雨量）
void countRain() {
    numClicksRain++;               //numClickRainに1を足す
  RainAction = 1;                  //雨量計によるスリープ復帰の判断用
}


boolean connectLTE(){
  pinMode(Sakura,OUTPUT);
  digitalWrite(Sakura,HIGH);       //Sakura電源ON
  Serial.println("sakura_ON");
  for(byte timeout=0; timeout<=110; timeout++){     //(110秒タイムアウト)
    Serial.print(sakuraio.getConnectionStatus());
    if( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ){
      Serial.print("\nConnect!:");
      if(millis()<Measuretime) sendDelay=51000; //millesが逆転していれば51秒を設定する
      else  sendDelay = millis() - Measuretime; //遅延時間を計測
      return true;//接続に成功
      break;
    }
    delay(1000);
  }
  Serial.println("\nConnect failed!");
  sendDelay = 0;
  return false; //接続に失敗
}


int WaterGauge10_ave(int n, int intarvalms,int Nmax){
    pinMode(water10Power,OUTPUT);   //10m超音波センサ電源ON
    digitalWrite(water10Power,LOW);  //10m超音波センサ電源ON
    delay(100);    //電源ON後300ms待ち 100+200
    temperature = temp_gain*(ReadSens_ch(2,4,50)-temp_offset); //温度測定電圧AD2の3回平均値(ch,回数,intarvalms) delay(200)も兼ねる
        int Wg =max_range; //10m 
        int Wgsum =0;
        int Wgmax =0;
        int Wgmin =max_range;
        int Distance2 = 0;
        int valid_count=0;            //有効データカウント
        invalid_Clong=0;            //無効データカウントクリア
        int DistanceX[n];       //有効水位距離（センサ端面）cm 測定log記録用
        for (int i = 0; i <n ; i++){ //最大Nmax回測定
            DistanceX[i]=0;
        }
        for (int i = 0; i <Nmax ; i++){ //最大Nmax回測定
            Wg = Water10_const*ReadSens_ch(0,4,intarvalms/4);      //電圧AD0の4回平均値(個別ch, 読取回数, intarvalms)            
            Serial.print(Wg);
            Serial.print(",");
            if(Wg >= max_range) {
              invalid_Clong++;
            }else if( Wg < 30) {
            }else{       //max_rangeより小さく30cmより大きい値のみ有効値と判断
              DistanceX[valid_count]=Wg;//有効データの配列への格納
              if (Wgmin > Wg) {         //最小値よりも小さい値の場合
                Distance2 = Wgmin;     //WWgminをDistance2に代入 
                Wgmin = Wg;           //Wgminに代入
              }else if(Distance2 > Wg) Distance2=Wg; //最小値よりも大きくDistance2より小さい値の場合Distance2に代入
              if (Wgmax < Wg) Wgmax = Wg;
              Wgsum += Wg;
              valid_count++;
            }
            if (valid_count>=n) break;                //有効データがn回測定できた時break
        }
       Serial.println();
       temperature = 0.5*temperature + 0.5*temp_gain*(ReadSens_ch(2,3,50)-temp_offset); //温度測定電圧AD2の3回平均値(ch,回数,intarvalms) delay(150)も兼ねる
       pinMode(water10Power,INPUT);   //10m超音波センサ電源OFF
       if(valid_count<=1){         //Nmax回中有効データが1以下ならmax_range
         Distance_ave=max_range;
       }else if(valid_count>=4)  Distance_ave=(Wgsum-Wgmax-Wgmin)/(valid_count-2); //最大と最小を外して平均 
       else Distance_ave=(Wgsum)/(valid_count);   //2～3なら単純平均
       int dis_temp;
       for(int m=0;m<valid_count-1;m++){
         for(int p=m+1;p<valid_count;p++){
           if(DistanceX[m]>DistanceX[p]){  //小さい順に並べ替え
             dis_temp=DistanceX[m];
             DistanceX[m]=DistanceX[p];
             DistanceX[p]=dis_temp;
           }
         }
       }
       if(valid_count>=6){
         Distance_ave=0;
         for(int p=2;p<valid_count-2;p++){
           Distance_ave +=DistanceX[p];
         }
         Distance_ave = Distance_ave/(valid_count-4);
       }
       if(valid_count==0) dis_temp = max_range;
       else dis_temp=DistanceX[(int)((valid_count-1)/2)];//真ん中の値,2のときは小さい方,3の時は真ん中,4の時は小さい方から2つ目
       if (invalid_Clong > Nmax-n && valid_count>=5) dis_temp=Distance2;//max_range無効回数>(最大測定回－有効回）の場合は小さい値から2つ目

       return dis_temp;//中心値
//       return Distance_ave;
}


int long_ave(int dt[]){     //最大値と最小値を除いたデータの平均値
        int Wmax =0;
        int Wmin = 1500;
        int sum = 0;
        int ave_count=0;
        int ave=max_range;
        for (int i = 0; i < measurec ; i++){ 
          if(dt[i] < max_range && dt[i] > 10) { //10より大きく、max_rangeより小さい値のみ有効な計測値として加算
            if (Wmax < dt[i]) Wmax = dt[i];  //最大値よりも大きい値の場合Wmaxに代入
            if (Wmin > dt[i]) Wmin = dt[i];  //最小値よりも大きい値の場合Wminに代入
            sum += dt[i];
            ave_count++;
          }
        }
        if (ave_count >= 4) ave=(sum-Wmax-Wmin)/(ave_count-2);  //有効計測数が4以上なら最大最小を除いて平均
        else if (ave_count == 0) ave=max_range;
        else if (ave_count < measurec ) ave = sum/ave_count;   //有効計測数が4未満の時は，有効データの全平均
      return ave;
}

boolean GPS_get(){
  Serial.begin(9600);//電流漏れ対策でTxをINPUTにしていたのをGPS実行前に戻す。
  pinMode(TRON_GPS,OUTPUT);   
  digitalWrite(TRON_GPS,HIGH);  //GPS ON　TRONと兼用のため
  Serial.println("GPSON");
  String response_g = "\n";
  int validcount = 0;
//  delay(3000);
  system_sleep();         //delayの代わりに8秒sleepを利用
  unsigned long timeg=0;     //GPS時刻データ
  latn = 0;
  lnge = 0;
  String Nlat;
  String Elng;
  float latnfa=0;
  float lngefa=0;
  for(int i=0 ; i<10000; i++){
    response_g = "";
    int wnum =600;   //この時間で待ち時間を調整 600->30秒
    while ((!Serial.available())&&(wnum != 0)) {
      delay(50);
      wnum--;
    }
    while (Serial.available() > 0){
      char c = Serial.read();
      response_g += c;
      if( c == '\n') break;    //改行コードまで
      if( c == '\r') break;    //改行コードまで
      if(response_g.length()<39) delay(1);
    }
    Nlat="";
    Elng="";
    int n=response_g.indexOf("$GPGGA");
    if(n==0 && response_g.length() >= 40){    //"$GPGGA"が先頭で40文字以上ある行のみ処理
      int m1 = response_g.indexOf(",",n);
      int m2 = response_g.indexOf(",",m1+1);
      String Tt=response_g.substring(m1+1,m2); 
      m1 = response_g.indexOf(",",m2+1);
      Nlat=response_g.substring(m2+1,m1);   //2つ目
      m2 = response_g.indexOf(",",m1+1);
      m1 = response_g.indexOf(",",m2+1);
      Elng=response_g.substring(m2+1,m1);   //4つ目
      if(Elng.length() >=8 ){  
        timeg=(long)Tt.toFloat();
        int latni  =Nlat.substring(0,2).toInt();
        float latnf=Nlat.substring(2).toFloat()/60.0;
        int lngei  =Elng.substring(1,3).toInt();
        float lngef=Elng.substring(3).toFloat()/60.0;
        if( 23<latni && latni< 47 && 23 < lngei && lngei <47){
          latn = latn + latni;
          lnge = lnge + lngei;
          latnfa = latnfa + latnf;
          lngefa = lngefa + lngef;
          validcount++;
        }
      }
    }
    if(validcount>=3)break;    //有効データ3回を取得
  }
  if(validcount>0){
    latn = (long)((float)latn/validcount*1000000)+(long)(latnfa/validcount*1000000+0.5);//floatのため有効桁7桁
    lnge = (long)((float)lnge/validcount*1000000)+(long)(lngefa/validcount*1000000+0.5);//東経の100の桁を除外して足し算
  }
  timeg = timeg + 90000;
  if(timeg>=240000) timeg = timeg - 240000;

  digitalWrite(TRON_GPS,LOW);  //GPS OFF TRONと兼用でノーマルLOWにしておく
  if(validcount>0)  return true; //GPSデータ受信成功
  else return  false; //GPSデータ受信失敗
}

float ReadSens_ch(int ch, int n, int intarvalms){
        int sva =0;
        for (int i = 0; i <n ; i++){ //n回平均
            delay(intarvalms);
            sva = (analogRead(ch) + sva);
        }
        return sva/n;
}

void WDT_setup8() {  // ウォッチドッグタイマーをセット。
  // WDTCSRにセットするWDP0-WDP3の値。9=8sec
  byte bb = 9;
  bb =bb & 7;                          // 下位3ビットをbbに
  bb |= (1 << 5);                     // bbの5ビット目(WDP3)を1にする
  bb |= ( 1 << WDCE );
  MCUSR &= ~(1 << WDRF);                // MCU Status Reg. Watchdog Reset Flag ->0
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1<<WDE);     // ウォッチドッグ変更許可（WDCEは4サイクルで自動リセット）
  // set new watchdog timeout value
  WDTCSR = bb;                          // 制御レジスタを設定
  WDTCSR |= _BV(WDIE);
} 

ISR(WDT_vect) {                         // WDTがタイムアップした時に実行される処理
  wdt_cycle++;                        
  if (wdt_cycle >= 440) { //8秒ｘ440(60分)以上経過したらソフトリセット
     software_Reset();
  }
}

void software_Reset(){
  asm volatile("  jmp 0");
}

