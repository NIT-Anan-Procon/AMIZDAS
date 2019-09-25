<?php
define('url','https://backend.sigfox.com/api/devices/739D7D/messages');
define('userID','5bee44802564321968522c47');
define('password','79c69d5c5a4f33d5d237a7bd34f90d0c');
$opts = [
	'http' => [
		'proxy' => 'tcp://proxy.anan-nct.ac.jp:8080',
		'request_fulluri' => true,
		'method' => 'GET',
		'header' => 'Authorization: Basic '. base64_encode(userID .':' . password)
	]
];

$json=file_get_contents(url, false, stream_context_create($opts));
$arr =json_decode($json, true);

$fp = fopen('csvfile.csv','w'); //ファイルオープン
$lat=0;
$lng=0;

for($i=count($arr["data"])-1;$i>=1;$i--){
	$time =$arr["data"][$i]["time"];
    $data =$arr["data"][$i]["data"];

	//緯度、経度
	if(strlen($data)==24){
		$data1 = hexdec(substr($data, 0,4));
		$data2 = hexdec(substr($data, 4,4));
		$data3 = hexdec(substr($data, 8,4));
		$data4 = hexdec(substr($data,12,4));
		$lat = (($data1)*65536+($data2))/1000000;
		$lng = (($data3)*65536+($data4))/1000000+100;
	}

    //水位データなど
    if(strlen($data)==20){
		$water_gage       = (hexdec(substr($data,2,2))*256+hexdec(substr($data,0,2)))/10.0;
		$radiation        = hexdec(substr($data,6,2))*256+hexdec(substr($data,4,2));          
		$vattery_voltage  = (hexdec(substr($data,10,2))*256+hexdec(substr($data,8,2)))/100.0; 
		$temperature      = (hexdec(substr($data,14,2))*256+hexdec(substr($data,12,2)))/100.0-50; 
		$water_gage2      = (hexdec(substr($data,18,2))*256+hexdec(substr($data,16,2)))/10.0;   
	
		//水位、日射量などのデータを配列に代入
		$csvdata = array(date('m/d H:i',$time+(3600*7)),$water_gage,$radiation,$vattery_voltage,$temperature,$water_gage2,$lat,$lng);
		fputcsv($fp,$csvdata);  //csv形式で書き込む
	}
}

fclose($fp);  //ファイルを閉じる
?>