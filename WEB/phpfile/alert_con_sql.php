<?php
    require_once('Config/SQLServer.php');
    $mysqli =  MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
    } 
    mysqli_set_charset($mysqli,'utf8');

    if(isset($_GET['name'])){
        $name =$_GET['name'];
        $select_module ="select module_ID as module_ID from water_level where water_level_name = '$name'";
        foreach($mysqli->query($select_module) as $module){
            $module_ID= $module['module_ID'];
        }
    }

    $water_level_sql = "SELECT attention,danger FROM alert where module_ID = '$module_ID' AND flag = 1";
    foreach($mysqli->query($water_level_sql) as $water_level){
        $attention = $water_level['attention'];
        $danger = $water_level['danger'];
    }

    $mysqli->close();
?>