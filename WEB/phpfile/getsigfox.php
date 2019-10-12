<?php
    ini_set("display_errors", 1);
	error_reporting(E_ALL);
	
	$mysqli = new mysqli('localhost','amizdas','Amizdas.1','amizdas');
	if($mysqli->connect_errno){
		echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
	}
	mysqli_set_charset( $mysqli, 'utf8');
	
	$water_sql = "SELECT standard_water FROM alert where module_ID = '739D7D' AND flag = 1";
	$waterdata=$mysqli->query($water_sql);
	$water = $waterdata->fetch_assoc();
	$standard_water = $water['standard_water'];

	$module_ID =['739D7D','739FFC','73E88A'];

	for($count=0;$count<count($module_ID);$count++){

		$url_sigfox = 'https://backend.sigfox.com/api/devices/'.$module_ID[$count].'/messages';
		$userID = '5bee44802564321968522c47';
		$password = '79c69d5c5a4f33d5d237a7bd34f90d0c';
		$opts = [
			'http' => [
				'method' => 'GET',
				'header' => 'Authorization: Basic '. base64_encode($userID .':' . $password)
			]
		];

		$json=file_get_contents($url_sigfox, false, stream_context_create($opts));
		$arr =json_decode($json, true);
		switch($count){
			case 0:
				$file = 'csvfile.csv';
				break;
			case 1:
				$file = 'csvfile1.csv';
				break;
			case 2:
				$file = 'csvfile2.csv';
				break;
		}

		$fp = fopen($file,'w'); //ファイルオープン
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
				$lng = (($data1)*65536+($data2))/1000000;
				$lat = (($data3)*65536+($data4))/1000000+100;
			}

			//水位データなど
			if(strlen($data)==20){
				$water_gage       = (hexdec(substr($data,2,2))*256+hexdec(substr($data,0,2)))/10.0;
				$radiation        = hexdec(substr($data,6,2))*256+hexdec(substr($data,4,2));          
				$vattery_voltage  = (hexdec(substr($data,10,2))*256+hexdec(substr($data,8,2)))/100.0; 
				$temperature      = (hexdec(substr($data,14,2))*256+hexdec(substr($data,12,2)))/100.0-50; 
				$water_gage2      = (hexdec(substr($data,18,2))*256+hexdec(substr($data,16,2)))/10.0;   
				$water_level = $water_gage-$standard_water;
				//水位、日射量などのデータを配列に代入
				$csvdata = array(date('m/d H:i',$time+(3600*7)),$water_level,$radiation,$vattery_voltage,$temperature,$water_gage2,$lat,$lng);
				fputcsv($fp,$csvdata);  //csv形式で書き込む
			}
		}
	fclose($fp);  //ファイルを閉じる
	}
?>