<?php
    require_once('Config/SQLServer.php'); //テストで../している
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
        echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
    }
    mysqli_set_charset($mysqli,'utf8');
    if(isset($_GET['name'])){
        $name = $_GET['name'];
    }
    $selectM_Name ="select module_ID from water_level where water_level_name = '$name'";
    foreach($mysqli->query($selectM_Name) as $module_ID){
        $id =$module_ID['module_ID'];
        $sql = "SELECT attention,danger FROM alert where module_ID='$id' AND flag = 1";
        $water_sql = "SELECT * FROM water_level where module_ID = '$id'";
        $sql_u = "SELECT attention,danger FROM alert where module_ID='$id' AND flag = 0";

        foreach($mysqli->query($water_sql) as $water_level){
            foreach($mysqli->query($sql_u) as $alert){
                $php_json_u = json_encode($alert);
            }
        }
        foreach($mysqli->query($water_sql) as $water_level){
            foreach($mysqli->query($sql) as $alert){
                $php_json = json_encode($alert);
            }
        }

        $user_sql = "SELECT address_ID FROM address where mailaddress = '1167120@st.anan-nct.ac.jp'";//ここに誰か特定する情報を追加する
        foreach ($mysqli->query($user_sql) as $address) {
          $add = $address['address_ID'];
          $user_alert_sql = "SELECT attention, danger FROM alert where module_ID='$id' AND address_ID = '$add and flag=0'";
          foreach ($mysqli->query($user_alert_sql) as $user_alert) {
            // code...
          }
        }


        //何人が水位計に登録しているのか数える
        $count_sql = "SELECT COUNT(*) as registration_count FROM alert where module_ID = '$id'";
        foreach($mysqli->query($count_sql) as $registration){
            // echo $registration['registration_count'];
        }
        $avg_danger = "SELECT AVG(danger) as danger FROM alert where module_ID = '$id' AND (flag = 0 OR flag = 1)";
        foreach($mysqli->query($avg_danger) as $ave){}

        $avg_attention = "SELECT AVG(attention) as attention FROM alert where module_ID = '$id' AND (flag = 0 OR flag = 1)";
        foreach($mysqli->query($avg_attention) as $average){
            // echo $average['attention'];
        }

    }
    $mysqli->close();
?>
