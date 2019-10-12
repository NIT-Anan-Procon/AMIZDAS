<?php
    require_once('../Config/SQLServer.php');
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
    }
    mysqli_set_charset( $mysqli, 'utf8');

    $time = time();
    $days_unix_time = date( 'Y-m-d\TH:i:sO',strtotime('-4 day',$time));
    echo $days_unix_time;

    $water_sql = "SELECT standard_water FROM alert where module_ID = 'uGqZClHAVHB2' AND flag = 1";
    $waterdata=$mysqli->query($water_sql);
    $water = $waterdata->fetch_assoc();
    $standard_water = $water['standard_water'];

    $module_ID = ['us9u0BjbxtTw'];
    $token = ['1uA76sUCweRPHcjqIrUJZxR7c3u5UJWxBxRCkJ0pL33w'];

    for($count=0;$count<count($module_ID);$count++){
        $url_sakura = 'https://api.sakura.io/datastore/v1/channels?after='.$days_unix_time.'&size=3000f&module='.$module_ID[$count].'&token='.$token{$count}.'&channel=2,7,15';

        $json = file_get_contents($url_sakura, false);
        $json = mb_convert_encoding($json, 'UTF8', 'ASCII,JIS,UTF-8,EUC-JP,SJIS-WIN');
        $arr = json_decode($json,true);

        $change_time;
        $csvdata  = array();
        $water_gage = array();
        $radiation = array();
        $vattery_voltage = array();
        $temperature = array();
        $water_gage2= array();
        $lat= array();
        $lng= array();
        $lat=0;
        $lng=0;

        switch($count){
            case 0:
                $file = 'csvfile_SS002.csv';
                break;
 
        }

        $fp = fopen($file,'w'); //ファイルオープン

        for($i=count($arr["results"])-1;$i>=1;$i--){
            $channel = $arr["results"][$i]["channel"];

            switch($channel){
                case 15://電池電圧
                    $vattery_voltage = $arr["results"][$i]["value"]; 
                    break;
                case 7://日射
                    $radiation = $arr["results"][$i]["value"];
                    
                    break;
                case 2://水位
                    $water_gage = $arr["results"][$i]["value"];
                    $water_level = $water_gage-$standard_water;
                    $time = $arr["results"][$i]["datetime"];
                    //時間を正規表現
                    $pattern="/\A(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})\.\d{9}+(Z)\z/";
                    $replacement="$1$2";
                    $ctime=preg_replace($pattern,$replacement,$time);
                    //unixtimeに変換
                    $t = date('U',strtotime($ctime));
                    $change_time = date('m/d H:i',$t+(3600*7)); 
                    $csvdata = array($change_time,$water_level,$radiation);
                    fputcsv($fp,$csvdata);  //csv形式で書き込む
                    break;

            }
        }
        fclose($fp);  //ファイルを閉じる
    }
?>