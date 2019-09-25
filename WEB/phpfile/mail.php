
<?php
//########map.htmlに埋め込んだらデータベースが更新さるたびに確認するため自動送信可能################
  //sql接続
  require_once('../Config/SQLServer.php');
  $mysqli =MySQLi();
  if($mysqli->connect_errno){
    echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
  }
  mysqli_set_charset( $mysqli, 'utf8');

  $mailaddress = array();
  $water_level = array();
  $alert = array();

  //alertテーブルからサブクエリとかで持ってきたほうがいい
  $address_sql = "SELECT * FROM address where address_ID = 1";
  $water_sql = "SELECT * FROM water_level where module_ID = '739D7D'";
  $alert_sql = "SELECT attention,danger FROM alert";

  //addressテーブルからデータを取得
  if ($address_result = $mysqli->query($address_sql)) {
      // 連想配列を取得
      while ($row = $address_result->fetch_assoc()) {
          $mailaddress = $row;
      }
      // 結果セットを閉じる
      $address_result->close();
  }
  //water_levelテーブルからデータを取得
  if($water_result = $mysqli->query($water_sql)) {
        // 連想配列を取得
        while ($row = $water_result->fetch_assoc()) {//結果セットにもう行がない場合にはNULLを返す
            $water_level = $row;
        }
        // 結果セットを閉じる
      $water_result->close();
  }
  //alertテーブルからデータを取得
  if($alert_result = $mysqli->query($alert_sql)) {
    // 連想配列を取得
    while ($row = $alert_result->fetch_assoc()) {//結果セットにもう行がない場合にはNULLを返す
        $alert = $row;
    }
    // 結果セットを閉じる
    $alert_result->close();
  }
  $mysqli->close();


  //言語と文字コードを設定
  mb_language("Japanese"); 
  mb_internal_encoding("UTF-8");

  //メールの情報を設定
  $mailto = $row['mailaddress'];
  $title = $water_level['water_level_name'] . "の水位について";
  //注意水位の場合のメール

  if($water_level['water_gage']<$line['attention']){
    $message = $water_level['water_gage'] . 'の水位が' . $alert['attention'] . 'に達しました。注意してください。';
    $option= "From: AMIZDAS";
    //メールの送信
    mb_send_mail($mailto,$title,$message,$option);
    echo 'メール送信しました';
    
  }else if($water_level['water_gage']<$line['danger']){
    $message = $water_level['water_gage'] . 'の水位が' . $alert['danger'] . 'に達しました。危険です。避難してください。';
    $option= "From: AMIZDAS";
    //メールの送信
    mb_send_mail($mailto,$title,$message,$option);
    echo 'メール送信しました';
  }

?> 