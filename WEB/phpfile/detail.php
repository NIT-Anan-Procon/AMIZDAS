<?php
    require_once('Config/SQLServer.php');
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
    }
    mysqli_set_charset($mysqli,'utf8');

    $sql = 'SELECT attention,danger FROM alert';
    $water_sql = "SELECT * FROM water_level where module_ID = '739D7D'";

    $attention = array();
    $danger = array();
    $alert = array();
    $water_level = array();

    if($water_result = $mysqli->query($water_sql)) {
        // 連想配列を取得
        while ($row = $water_result->fetch_assoc()) {
            $water_level = $row;
        }
        // 結果セットを閉じる
        $water_result->close();
    }

    if($result = $mysqli->query($sql)) {
        // 連想配列を取得
        while ($row = $result->fetch_assoc()) {
            $alert = $row;
        }
        // 結果セットを閉じる
        $result->close();
    }

    $mysqli->close();
    //json形式にしてjsに送る
    $php_json = json_encode($alert);
?>