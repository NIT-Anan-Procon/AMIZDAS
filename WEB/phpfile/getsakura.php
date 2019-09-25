<?php
    define('url','https://api.sakura.io/datastore/v1/channels?size=500&module=uGqZClHAVHB2&token=ed71fe25-af48-4d6a-a546-c864c907ced4&channel=2,7,1,15,11,13,12');
    $opts = [
        'http' => [
            'proxy' => 'tcp://proxy.anan-nct.ac.jp:8080',
            'request_fulluri' => true,
        ]
    ];
    $json = file_get_contents(url,false, stream_context_create($opts));
    $json = mb_convert_encoding($json, 'UTF8', 'ASCII,JIS,UTF-8,EUC-JP,SJIS-WIN');
    $arr = json_decode($json,true);

    $csvdata  = array();

    $fp = fopen('csvfile_sakura.csv','w'); //ファイルオープン

    for($i=count($arr["results"])-1;$i>=1;$i--){
        $channel = $arr["results"][$i]["channel"];

        switch($channel){
            case 2://水位
                $water_gage = $arr["results"][$i]["value"];
                $time = $arr["results"][$i]["datetime"];
                //時間を正規表現
                $pattern="/\A(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})\.\d{9}+(Z)\z/";
                $replacement="$1$2";
                $ctime=preg_replace($pattern,$replacement,$time);
                //unixtimeに変換
                $t = date('U',strtotime($ctime));
                $change_time = date('m/d H:i',$t+(3600*7));
                break;
            case 7://日射
                $radiation = $arr["results"][$i]["value"];
                break;
            case 1://温度
                $vattery_voltage = $arr["results"][$i]["value"];
                break;
            case 15://電池電圧
                $temperature = $arr["results"][$i]["value"];
                break;
            case 11://降水量
                $water_gage2 = $arr["results"][$i]["value"];
                $csvdata = array($change_time,$water_gage,$radiation,$vattery_voltage,$temperature,$water_gage2,$lat,$lng);
                fputcsv($fp,$csvdata);  //csv形式で書き込む
                break;
            case 13://東経
                $lat = $arr["results"][$i]["value"];
                break;
            case 12://北緯
                $lng = $arr["results"][$i]["value"];
                break;
        }
    }
    fclose($fp);  //ファイルを閉じる
?>