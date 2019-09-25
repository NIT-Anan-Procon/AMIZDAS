<?php
    //webスクレイピング
    require_once("./phpQuery-onefile.php");
    $html = file_get_contents("http://www.river.go.jp/kawabou/ipSuiiKobetu.do?obsrvId=0921700400080&gamenId=01-1003&stgGrpKind=survForeKjExpl&fldCtlParty=no&fvrt=yes");
    $doc = phpQuery::newDocument($html);
    $time = $doc[".tb1td1RightNoUline"]->text();
    $water = $doc[".tb1td2RightNoUline"]->text();

    //正規表現　置換
    $time_regular = preg_replace("/\\\r\\\n|\\\r|\\\n|\t|\s/","",$time);

    $timestr = preg_replace('/[0-9][0-9]\/[0-9][0-9]/',"",$time_regular);
    $datestr = preg_replace('/[0-9][0-9]:[0-9][0-9]/',"",$time_regular);

    $time_array = str_split($timestr, 5);
    $month_array = str_split($datestr, 5);

    var_dump($time_regular);

?>