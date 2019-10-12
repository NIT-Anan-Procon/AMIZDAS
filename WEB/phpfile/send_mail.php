<?php
    ini_set("display_errors", 1);
    error_reporting(E_ALL);
    require_once('../Config/SQLServer.php');
    $mysqli =  MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
    }
    mysqli_set_charset($mysqli,'utf8');

    mb_language("Japanese");
    mb_internal_encoding("UTF-8");
    
    // if(isset($_GET['name'])){
    //     $name =$_GET['name'];
    // }
    // echo $name;

    //メール本文の受け取り
    $message = $_POST['message'];
    $name = $_POST['name'];
    

    $select_module ="select module_ID as module_ID from water_level where water_level_name = '$name'";
    //モジュールのIDを取得
    foreach($mysqli->query($select_module) as $module){
        $id = $module['module_ID'];
        $address_sql = "SELECT address_ID as address_ID FROM alert where module_ID = '$id'";
        //アドレスのIDを取得
        foreach($mysqli->query($address_sql) as $address_ID){
            $address = $address_ID['address_ID'];
            $mailaddress_sql = "SELECT mailaddress FROM address where address_ID = '$address'";
            // echo $address;
            //メールアドレス取得
            foreach($mysqli->query($mailaddress_sql) as $mailaddress_ID){
                $mailaddress = $mailaddress_ID['mailaddress'];
                

                $mailto = $mailaddress;
                $title = $name. "の水位について";
                //差出人設定
                $from_email = 'mailserver1212@gmail.com';
                $from_name = '水位計';
                $headers = 'From: ' . mb_encode_mimeheader($from_name, 'ISO-2022-JP') . ' <' . $from_email . '>';
                
                echo $mailto;
                echo $title;
                echo $message;

                if(mb_send_mail($mailto, $title, $message,$headers)){
                    echo "メールを送信しました";
                } else {
                    echo "メールの送信に失敗しました";
                }
            }
        }
    }
    
    $mysqli->close();
?>
