/*watergaugeV2_HC10_Fx.ino   SakuraA3基板 10m 温度補正有り GPS Sigfox */
#define VernNo "Ver1.1"
/* スリープ中の雨量計検知時には一旦スリープを解除し各センサ出力を測定しサーバへ送信し再びスリープ*/
//制御フラグ
boolean Initialization=1;  //1:Initialization

//WDTタイムアウト
#include <avr/wdt.h>
volatile byte wdt_cycle = 0;           // WDTタイムアウトカウント
#define timeout_count10 72           //10分(72) WDTソフトリセット時間設定 RTC無し前提
//#define timeout_count5 16             //test用 8秒 WDTソフト割り込み時間設定

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
//Sigfox
//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

#define PIN_RAIN 2                   //PIN_RAINをデジタルI/Oピン2に設定

#define Sigfox 3 //Sigfoxシールドの電源ON/OFF LOWでON
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
float temperature = 0;               //気温を初期化
#define temp_gain 0.1653     // ℃・LSB =3.3/1024*1000/19.5
#define temp_offset  124   // 0℃ 0.4V  0.4/3.3*1024=124
boolean send_Flg=0;         //送信フラグ送信時1
boolean night=1;          //日射量が0ならnight=1
boolean GPSflag=0;        //GPSflag GPS測定失敗ならfalse (0)
unsigned long latn=0;      //北緯データ
unsigned long lnge=0;      //東経データ

void setup() {  //  Will be called only once.
  Serial.begin(9600);  Serial.println(F("Running setup..."));
  pinMode(TRON_GPS,OUTPUT);   
  digitalWrite(TRON_GPS,LOW);    //TR_OFF,GPS＿OFF
  pinMode(PIN_RAIN, INPUT_PULLUP);                         //PIN_RAIN(デジタルI/Oピン2)を入力ピンに設定
  attachInterrupt(0, countRain, FALLING);           //割り込み番号1:FALLINGピンの状態がHIGHからLOWに変わったときにcountRain関数を呼び出し
  pinMode(Sigfox,OUTPUT);
  digitalWrite(Sigfox,LOW);    //sigfoxの電源ON

  mySerial.begin(9600);
  Serial.println("Connect to the Sigfox Breakout board...");
  Serial.println("AT$I=10 : get Device ID");
  Serial.println("AT$SF=[payload] : SEND SIGFOX MESSAGE");
  mySerial.print("AT$I=10\r");
  Serial.print("Device ID : ");
  byte wnum =20;   //この時間で待ち時間を調整 20->2秒
  while ((!mySerial.available())&&(wnum != 0)) {
    delay(100);
    wnum--;
  }
  while ( mySerial.available()) {
     Serial.write(mySerial.read());
  }

  digitalWrite(Sigfox,HIGH);//Sigfoxの電源OFF
  pinMode(Sigfox,INPUT);//Sigfoxの電源OFF
  Serial.println("---Start Loop---");
  WDT_setup8();                          // 8秒のWDT設定
  Serial.end();    // Txの漏れ電流対策
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
}

void loop()
{
  Time_n++;
  digitalWrite(TRON_GPS,HIGH); //TR ON  2Wパネル日射量測定の為太陽電池出力短絡ON　GPS ON兼用
  delay(50);
  int Radiation=Radiation_constant*ReadSens_ch(3,4,50); //日射量:2Wパネル日射量AD3の5回平均値(個別ch, 読取回数, intarvalms)
  if (Initialization == 1) {   //Arduino起動後に雨量計が1カウント誤検知するため、ループ1回目は強制的に雨量計・風速計のカウント数を0にする。
    numClicksRain = 0;
  }else   digitalWrite(TRON_GPS,LOW);  //TR OFF  太陽電池出力短絡OFF　GPS OFF 初回のみOFFしない。
  if( RtcCount%6==0){          //1時間に1回日射量チェック
    if( Radiation==0 ) NoRadiationCount++; //日射量が0ならNoRadiationCountをインクリメント
    else NoRadiationCount=0;               //日射量があればNoRadiationCountをクリア
  }

  int DistA[measurec];           //4秒間隔で7回測定値保存用
  for(int m=0; m<measurec; m++){   //4秒間隔で7回測定
    unsigned long Measuretime = millis(); //測定終了時刻を保存する
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
 
  String response = "\n";
  //  Begin SIGFOX Module Loop
  if(RtcCount%6==0 || send_Flg==1){       //(RtcCount%6=0）なら送信
    pinMode(Sigfox,OUTPUT);          
    digitalWrite(Sigfox,LOW);        //sigfoxの電源ON
//    Serial.println("Sigfox_ON");
    Eneloop=Eneloop_const*ReadSens_ch(1,4,50);      //Eneloop電圧AD1の4回平均値(個別ch, 読取回数, intarvalms)(delay200も兼ねている)

    //  Set SIGFOX msg
    String msg = ToHex(Distance) //  int to 2 byte: Distance
     + ToHex(Radiation)    //  int to 2 byte: Radiation
     + ToHex((int)(Eneloop*100.0))    //  int to 2 byte: Eneloop * 100
     + ToHex((int)((temperature+50)*100))   //  int to 2 byte: temperature
     + ToHex(Rainfall);      //  int to 2 byte: Rainfallの10倍値送信
  
    if (Distance < max_range ) Distance_send=Distance; //Distance_sendを更新
 
    //  Send the message.
    response=Sigfox_Send_msg(msg);

    Serial.begin(9600);//電流漏れ対策でTxをINPUTにしていたのを戻す。
    //GPSを1日2回測定、日の出と日の入りのタイミング。
    if( (Radiation > 0 && night==1) || GPSflag==false || (Radiation == 0 && night==0) ){
      GPSflag = GPS_get();//GPS取得,夜から日射量が出始めたら測定、前回失敗なら測定
      Serial.end();    // Txの漏れ電流対策
    }
    if(Radiation > 0) night=0;   //日射量があればnightフラグは0、無ければ1（true）
    else night=1;

    if(GPSflag==true){    //  GPS取得成功なら送信  
    //  Set SIGFOX msg
      unsigned long ul3= (long)((temperature+50)*10)
                       +( (long)((Eneloop-3)*50)<<11)
                       +( (long)(Radiation) <<19);
      //  Send message  GPS_data as a SIGFOX message.
      msg = bit32Hex(latn)     //  long to 32bits:北緯  dd.ddddd*100
           + bit32Hex(lnge)     //  long to 32bits: 東経 dd.ddddd*100   100の桁を省略して送信
           + bit32Hex(ul3);    //  long to 32bits: temperature,Eneloop,Radiation
  
      //  Send the message.
      response=Sigfox_Send_msg(msg);
    }
  }


//Eneloop電池電圧が4.8V以上で日射量があるとき、電流測定回路をONにして過充電を防止する。
  Eneloop=Eneloop_const*ReadSens_ch(1,8,50);      //Eneloop電圧AD1の4回平均値(個別ch, 読取回数, intarvalms)(delay400も兼ねている)
  if (Eneloop>4.8 ){

    digitalWrite(TRON_GPS,HIGH);//TR ON  電流測定回路ON(GPS電源と兼用) 
  }
  digitalWrite(Sigfox,HIGH);//Sigfoxの電源OFF
  pinMode(Sigfox,INPUT);//Sigfoxの電源OFF
  if( NoRadiationCount > 24)  software_Reset(); //日射量が24時間連続で0ならリセットし、setup内で日射量を検知するまでスリープ
  
//sleepからの復帰が雨量計の場合およびWDTの場合は再度スリープに戻る。
  do{
    RainAction = 0; //スリープ後のスリープ復帰判断用にkを初期化  //anan Sleep復帰から計測～送信完了までの間にRainAction = 1になった場合の対応
    //Sleep mode Setup
    system_sleep();
  }while(RainAction !=0 || wdt_cycle < timeout_count10);  // timeout_count10(x8.192秒)以上経過したら抜ける
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


String Sigfox_Send_msg(String msg){
  //  Send the message.
    while ( mySerial.available()) mySerial.read();  //送信前の空読み
//    Serial.println("AT$SF=" + msg + "\r");
    mySerial.println("AT$SF=" + msg + "\r");  //msg.send
    system_sleep();
    int wnum =300;   //この時間で待ち時間を調整 300->30秒
    while ((!mySerial.available())&&(wnum != 0)) {
      delay(100);
      wnum--;
    }
    String response = "\n";
    while ( mySerial.available()) {
       char readchar = mySerial.read();
       response += readchar;
    }
    return response;
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

String bit32Hex(unsigned long ul) {  //longのhex値順のまま32bit文字列に変換
  byte * b = (byte *) &ul;
  String bytes;
  for (int i=3; i>=0; i--) {         //longの順番のまま32bit文字列に
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String ToHex( int ul) {  //intのhex値を16bit文字列（下位、上位順）に変換
  byte * b = (byte *) &ul;
  String bytes;
  for (int i=0; i<2; i++) {         //下位上位の順番で文字列に
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
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
  //GPSによるRtcCount合わせ処理
  RtcCount=(byte)((int)((timeg%10000)/100))/10;   //10分刻みのRtcCountを設定
  wdt_cycle=(int)((((int)( (timeg%10000)/100)%10)*60+timeg%100) /8.192+0.5);//次の10分までのためのWDTを設定
  if(wdt_cycle > timeout_count10-7){
    wdt_cycle=0;      //残り56秒以内ならクリアして10分後に測定
    RtcCount=(RtcCount++)%6;   //RtcCountもインクリメント
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

