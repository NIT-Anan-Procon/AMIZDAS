<?php
    ini_set("display_errors", 1);
    error_reporting(E_ALL);
    //sql接続
    require_once('../Config/SQLServer.php');
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
    }
    mysqli_set_charset( $mysqli, 'utf8');

    function Get_Water_level($module_ID,$data){
        $mysqli =MySQLi();
        mysqli_set_charset( $mysqli, 'utf8');
        if($data=='name'){
            $select = "select water_level_name from water_level where module_ID ='$module_ID'";
            foreach($mysqli->query($select) as $water_gage){
                return $water_gage['water_level_name'];
            }
        }else{
            $select = "select water_gage from water_level where module_ID ='$module_ID'";
            foreach($mysqli->query($select) as $water_gage){
                return $water_gage['water_gage'];
            }
        }

    }

    function Get_MailAddress($address_ID){
        $mysqli =MySQLi();
        mysqli_set_charset( $mysqli, 'utf8');
        $selectAD = "select mailaddress from address where address_ID ='$address_ID'";
        foreach($mysqli->query($selectAD) as $mailaddress){
            return $mailaddress['mailaddress'];
        }
    }
    //alertテーブルからサブクエリとかで持ってきたほうがいい
    $alert_sql = "SELECT * FROM alert";
    //alertテーブルからデータを取得
    $alert_result = $mysqli->query($alert_sql);
    foreach($alert_result as $Alert){
        // echo(Get_Water_Gage('739D7D'));
        // echo(Get_MailAddress(1));
        //言語と文字コードを設定
        mb_language("Japanese"); 
        mb_internal_encoding("UTF-8");

        //メールの情報を設定
        $mailto = Get_MailAddress($Alert['address_ID']);
        $module_name= Get_Water_level($Alert['module_ID'],'name');
        $title = $module_name. "の水位について";
        //差出人設定
        $from_email = 'mailserver1212@gmail.com';
        $from_name = '水位計';
        $headers = 'From: ' . mb_encode_mimeheader($from_name, 'ISO-2022-JP') . ' <' . $from_email . '>';
        $Now_Water_Gage = Get_Water_level($Alert['module_ID'],'water_gage');
        echo $mailto;
        echo $title;
        echo $Now_Water_Gage;
        echo "</br>";
        if($Now_Water_Gage>$Alert['attention']){
            $message = $Now_Water_Gage . 'の水位が' . $Alert['attention'] . 'に達しました。注意してください。';
            //メールの送信
            if(mb_send_mail($mailto,$title,$message,$headers)){
            echo 'メール送信しました';
            }else{
            echo 'メール送信失敗';
            }
        }else if($Now_Water_Gage>$Alert['danger']){
            $message = $Now_Water_Gage . 'の水位が' . $Alert['danger'] . 'に達しました。危険です。避難してください。';
            $option= "From: AMIZDAS";
            //メールの送信
            mb_send_mail($mailto,$title,$message,$option);
            echo 'メール送信しました';
        }
    }

?> 