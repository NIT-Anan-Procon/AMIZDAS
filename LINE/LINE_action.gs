// line developersに書いてあるChannel Access Token
var access_token = "GT6LdgZAwCaOc87PjP7UX96q3WkVXYrFPbR88PhVopS1jvXj/p+FrvVmMude9EVbuWjChQWTtdK0liZND7b0Hp1qXBrcRnNZWYz9RX1ThSYbap1Md/yFLRh/LWoneFAYAOcUT0NW8VqRwKbYqBm7fQdB04t89/1O/w1cDnyilFU="
// pushしたいときに送る先のuser_id or group_idを指定する
var to = "U3ac2429722f5d0bc7b251639ef11005e"
// postされたログを残すスプレッドシートのid
var spreadsheet_id = "1ec8QAfMpakl1yKEyWmS08nJMjGzaO0M6OsxSAxgFNvs"

var SS = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1ec8QAfMpakl1yKEyWmS08nJMjGzaO0M6OsxSAxgFNvs/"); //LogのSpreadsheetのURL
var sheet = SS.getSheetByName("log"); //Spreadsheetのシート名（タブ名）
var lastrow = sheet.getLastRow();
var lastcol = sheet.getLastColumn();
var sheetdata = sheet.getSheetValues(1, 1, lastrow, lastcol);


/**
 * 指定のuser_idにpushをする
 */
function push(text) {
  var url = "https://api.line.me/v2/bot/message/push";
  var headers = {
    "Content-Type" : "application/json; charset=UTF-8",
    'Authorization': 'Bearer ' + access_token,
  };

  var postData = {
    "to" : to,
    "messages" : [
      {
        'type':'text',
        'text':text,
      }
    ]
  };

  var options = {
    "method" : "post",
    "headers" : headers,
    "payload" : JSON.stringify(postData)
  };

  return UrlFetchApp.fetch(url, options);
}


/**
 * reply_tokenを使ってreplyする
 */
function reply(data) {
  var url = "https://api.line.me/v2/bot/message/reply";
  var headers = {
    "Content-Type" : "application/json; charset=UTF-8",
    'Authorization': 'Bearer ' + access_token,
  };

  var postData = {    
    "replyToken" : data.events[0].replyToken,
    "messages" : [
      {
        'type':'text',
        //'text': ((e.message.type=="text")? e.message.text : "Text以外"),
        'text':　"分かりました。先生に「" + data.events[0].message.text + "」とお伝えしておきますね。",
      }
    ]
  };

  var options = {
    "method" : "post",
    "headers" : headers,
    "payload" : JSON.stringify(postData)
  };

  return UrlFetchApp.fetch(url, options);
}


/**
 * postされたときの処理
 */
function doPost(e) {
  var Lastrow = sheet.getLastRow();
  var Lastcol = sheet.getLastColumn();
  
  var json = JSON.parse(e.postData.contents);
  var data = SpreadsheetApp.openById(spreadsheet_id).getSheetByName('log').getRange(Lastrow+1, 1).setValue(json.events);
  
  Logger.log(SpreadsheetApp.openById(spreadsheet_id).getSheetByName('log').getRange(Lastrow+1, 1));
  
  reply(json);
}


/**
 * pushをしてみる
 * 返信する
 */
function test() {
  push('あなたの名前を教えて下さい');
}



/*
 * LINE Notifyでの通知の内部処理
 * メッセージ送信の処理
 */
function sendHttpPost(message){
  var token = 'IAJcCqqde2FHWCIoM9sEAo5F9AKWj2FfJzSV9Czx9SB'; // トークンを入力
  var options =
   {
     "method"  : "post",
     "payload" : "message=" + message,
     "headers" : {"Authorization" : "Bearer "+ token}
   };

   UrlFetchApp.fetch("https://notify-api.line.me/api/notify",options);
}


/*
 * LINE Notifyでの通知の内部処理
 * MAPをLINEに送信する
 */
function sendHttpPostImage(message, blob){
  var token = 'IAJcCqqde2FHWCIoM9sEAo5F9AKWj2FfJzSV9Czx9SB';
  var formData = {
   'message' : message,
   'imageFile': blob  // 地図画像を添付
  }
  var options =
   {
     "method"  : "post",
     "payload" : formData,  // message, imageFile を formData としてPost
     "headers" : {"Authorization" : "Bearer "+ token}
   };

   UrlFetchApp.fetch("https://notify-api.line.me/api/notify",options);
}


/*
 * LINE Notifyでの通知を実行
 */
function myFunction(){
  var S_SS = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1SH03MTiW6xP8ncGaRucoebyz4cVtnjQlKxeW1UVuMi4/"); //水位計のSpreadsheetのURL
  var S_sheet = S_SS.getSheetByName("データページ"); //Spreadsheetのシート名（タブ名）
  var S_lastrow = S_sheet.getLastRow();
  var S_lastcol = S_sheet.getLastColumn();
  
  var water_level = "\n 水田の水位 " + S_sheet.getRange(S_lastrow, 2).getValue() + " cm"; //最新の水位データ
  
  var latitude = S_sheet.getRange(S_lastrow-1, 9).getValue();//緯度
  var longitude = S_sheet.getRange(S_lastrow-1, 8).getValue();//経度
  
  var message="Google App Srciptから送信" ;
  
  sendHttpPost(water_level); //メッセージはここで一度送信

  
  //以下MAP情報の作成と送信
  // StaticMap の基本設定
  var map = Maps.newStaticMap()
    .setSize(600, 600)  // 画像サイズの指定 (Max: 600x600)
    .setLanguage('ja')  // 言語の設定
    .setMobile(true)  // モバイル端末向けの地図
    .setMapType(Maps.StaticMap.Type.HYBRID)  // 航空写真＋通常

  // StaticMap上にマーカーを設置
  map.addMarker(latitude, longitude)  // 緯度経度を指定する場合 
  
  var mapBlob = map.getBlob()// Blobとして画像を取得
  var mapUrl = map.getMapUrl()// StaticMap へのパスを取得
    
  sendHttpPostImage(mapUrl, mapBlob)// MAPデータを送信
}