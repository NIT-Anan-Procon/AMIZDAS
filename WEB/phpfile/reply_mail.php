<?php 
  //sql接続
  require_once('../Config/SQLServer.php');
  $mysqli =MySQLi();
  if($mysqli->connect_errno){
    echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
  }
  mysqli_set_charset( $mysqli, 'utf8');

  $mailaddress = array();
  $address_sql = "SELECT * FROM address where address_ID = 1";

 //addressテーブルからデータを取得
  if ($address_result = $mysqli->query($address_sql)) {
      // 連想配列を取得
      while ($row = $address_result->fetch_assoc()) {
          $mailaddress = $row;
      }
      // 結果セットを閉じる
      $address_result->close();
  }

  mb_language("Japanese"); 
  mb_internal_encoding("UTF-8");

  //メールの情報を設定
  $mailto = '1167120@st.anan-nct.ac.jp';
  $title = '水位計アラート設定について';
  $message = $water_level['water_gage_name'] . "にあなたのメールアドレスが仮登録されました。\r\n下のリンクをクリックするとメールアドレスを登録することができます。\r\nhttp://192.168.10.162:8080";

  //差出人設定
  $from_email = 'mailserver1212@gmail.com';
  $from_name = '水位計';
  $headers = 'From: ' . mb_encode_mimeheader($from_name, 'ISO-2022-JP') . ' <' . $from_email . '>';
  
    //メールの送信
  if(mb_send_mail($mailto,$title,$message,$headers)){
    echo 'メール送信しました';
  }else{
    echo 'メール送信失敗';
  }
?>