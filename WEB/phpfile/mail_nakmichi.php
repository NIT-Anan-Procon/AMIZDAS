<?php
    ini_set("display_errors", 1);
    error_reporting(E_ALL);
    //########map.htmlに埋め込んだらデータベースが更新さるたびに確認するため自動送信可能################
    //sql接続
    require_once('../Config/SQLServer.php');
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
    }
    mysqli_set_charset( $mysqli, 'utf8');
    session_start();
    
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
        mysqli_set_charset($mysqli, 'utf8');
        $selectAD = "select mailaddress from address where address_ID ='$address_ID'";
        foreach($mysqli->query($selectAD) as $mailaddress){
            return $mailaddress['mailaddress'];
        }
    }
    function Get_Flag($Water_gage,$Alert_ID){
        $mysqli =MySQLi();
        mysqli_set_charset( $mysqli, 'utf8');
        $selectAl = "select * from past_alertInfo where alert_ID ='$Alert_ID'";
        $PastAlert=$mysqli->query($selectAl);
        if(!($PastAlert==NULL)){
            foreach($PastAlert as $AlertInfo){
                $changeW_G = $Water_gage - $AlertInfo['past_water_level'];
                $nextattention =$AlertInfo['nextattention'];
                $nextdanger =$AlertInfo['nextdanger'];
                if($changeW_G>=$nextdanger){
                    return 1;
                }else if($nextdanger<=0&&$changeW_G<=$nextdanger){
                    return -1;
                }else{

                    switch($nextattention){
                        case $nextattention <= 0:
                            if($changeW_G<$nextattention){
                                return -1;
                            }
                            break;
                        case $nextattention >=0:
                        if($changeW_G>$nextattention){
                            return 1;
                        }
                        break;
                    }
                }
            }
            return 0;
        }
    }
    //alertテーブルからサブクエリとかで持ってきたほうがいい
    $alert_sql = "SELECT * FROM alert";
    //6ttalertテーブルからデータを取得
    $alert_result = $mysqli->query($alert_sql);
    // session_destroy();
    foreach($alert_result as $Alert){
        $AlertID=$Alert['alert_ID'];
        $selectPA ="select alert_ID from past_alertinfo where alert_ID='$AlertID'";
        $pastAlert = $mysqli->query($selectPA);
        
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

        if($pastAlert->fetch_assoc()){
            $flag=Get_Flag($Now_Water_Gage,$AlertID);
        }else{
            $flag=1;
            $updateflag=true;
        }

        
        
        var_dump($Alert);
        echo "</br>";
        echo $flag;
        if($flag==1){
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
        }else if($flag==(-1)){
            $message = $module_ID. 'の水位が' . $Now_Water_Gage . 'に戻りました。';
                //メールの送信
                if(mb_send_mail($mailto,$title,$message,$headers)){
                echo 'メール送信しました';
                }else{
                echo 'メール送信失敗';
                }
        }
        
    }
    $nextattention=$Alert['attention']-$Now_Water_Gage;
        $nextdanger=$Alert['danger']-$Now_Water_Gage;
        if($updateflag){
            $update_oldAL ="update past_alertInfo set nextattention='$nextattention ,nextdanger ='$nextdanger',past_water_level='$Now_Water_Gage'where alert_ID= '$AlertID'";
            $Doupdate = $mysqli->query($update_oldAL);
        }else{
            $insert_oldAL ="insert into past_alertinfo(alert_ID,nextattention,nextdanger,past_water_level) values('$AlertID','$nextattention','$nextdanger','$Now_Water_Gage')";
            $Doinsert = $mysqli->query($insert_oldAL);
            
        }   

?> 