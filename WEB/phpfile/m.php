<?php
	require_once('Config/SQLServer.php');
            $mysqli =  MySQLi();
            if($mysqli->connect_errno){
                echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
            }
	    mysqli_set_charset($mysqli,'utf8');

	$water="SELECT water_gage from water_level";
	$danger="SELECT danger from alert";
	$attention="SELECT attention from alert";
	$lat="SELECT lat from water_level";
	$lng="SELECT lng from water_level";
	$sum="SELECT module_ID from water_level";

	$data1=$mysqli->query($water);
	$data2=$mysqli->query($danger);
	$data3=$mysqli->query($attention);
	$data4=$mysqli->query($lat);
	$data5=$mysqli->query($lng);
	$data6=$mysqli->query($sum);

	foreach($data1 as $value){
		$water_gage_data[]=$value['water_gage'];
	}
	foreach($data2 as $value){
		$danger_data[]=$value['danger'];
	}
	foreach($data3 as $value){
		$attention_data[]=$value['attention'];
	}
	foreach($data4 as $value){
		$lat_data[]=$value['lat'];
	}
	
	foreach($data5 as $value){
		$lng_data[]=$value['lng'];
	}
	foreach($data6 as $value){
		$cnt[]=$value['module_ID'];
	}
	
	$array_count=count($cnt);


	function json_safe_encode($data=array("data")){
		return json_encode($data, JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_HEX_QUOT);
	}

	$prefecture=$_GET['pref'];
	session_start();
	$_SESSION['string']=$prefecture;
	
?>
<script src="js/jquery.min.js"></script>

<script id="script" type="text/javascript" src="./js/map.js"
 data-param1='<?php echo json_safe_encode($water_gage_data);?>'
 data-param2='<?php echo json_safe_encode($danger_data);?>'
 data-param3='<?php echo json_safe_encode($attention_data);?>'
 data-param4='<?php echo json_safe_encode($lat_data);?>'
 data-param5='<?php echo json_safe_encode($lng_data);?>'
 data-param6='<?php echo json_safe_encode($array_count);?>'
 data-param7='<?php echo json_safe_encode($prefecture);?>'
>
</script>
