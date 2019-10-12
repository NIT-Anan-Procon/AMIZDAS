<?php
require_once('../Config/SQLServer.php');
$mysqli =  MySQLi();
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
} 
mysqli_set_charset($mysqli,'utf8');
$id = $_POST['address'];
if(isset($_GET['name'])){
    $name =$_GET['name'];
    $select_module ="select module_ID as module_ID from water_level where water_level_name = '$name'";
    foreach($mysqli->query($select_module) as $module){
        $module_ID= $module['module_ID'];
    }
}

function Get_flag($flag){
    var_dump($flag);
    if(!($flag==NULL)){
        foreach($flag as $row){
            // var_dump($row);
            if($row==1){
                return 1;
            }
        }
    }
    return 0;
}


if($_POST["update"]){
    // var_dump($id);
    foreach($id as $row){
        $selectAD ="select mailaddress from address where mailaddress ='$row'";
        $CountAD = $mysqli->query($selectAD);
        if($CountAD->fetch_assoc()){        
            // var_dump($CountAD->fetch_assoc());    
        }else{
            $insertAD ="insert into address(mailaddress) values('$row')";
            $stmt = $mysqli->query($insertAD);
            
            echo("メールアドレスを登録しました");
        }
        $select ="select address_ID as AD_ID from address where mailaddress='$row'";
        foreach($mysqli->query($select) as $addressID){ 
            $AD_ID=$addressID['AD_ID'];
            //$selectAD ="select mailaddress from address where mailaddress ='116720@st.anan-nct.ac.jp'";
            
            $selectCa ="select address_ID from alert where address_ID ='$AD_ID'";
            $selectCm ="select module_ID from alert where module_ID ='$module_ID'";
            $selectFl ="select flag from alert where address_ID ='$AD_ID' and module_ID ='$module_ID'";
            $selectAl ="select count(*) as num from alert";
            $CountCa = $mysqli->query($selectCa);
            $CountCm = $mysqli->query($selectCm);
            $CountFl = $mysqli->query($selectFl);
            $CountAl = $mysqli->query($selectAl);
            $test=$CountAl;
            $flag=array();
            foreach($CountFl as $row){
                array_push($flag,$row['flag']);
            }

            $AlertNum=$CountAl->fetch_assoc();
            if(!($AlertNum==0)){
                if(Get_flag($flag)==1){
                    if($CountCa->fetch_assoc()){
                        if($CountCm->fetch_assoc()){
                            $judge=1;
                            
                        }else{
                            $judge=0;
                        
                        }
                    }else{
                        $judge=0;
                        
                    }
                }else{
                    $judge=0;
                    
                }
            }else{
                $judge=0;
            }

            $attention = $_POST['attention'];
            $danger = $_POST['danger'];
            $standard_water = $_POST['standard_water'];//ここ追加
            // $insert = "INSERT into alert(module_ID,address_ID,attention,danger,flag) values('$module_ID','$AD_ID','$attention','$danger',1)";
            // $update = "UPDATE alert set attention = '$attention',danger ='$danger' where module_ID = '$module_ID' and address_ID ='$AD_ID' and flag=1";
            $insert = "INSERT into alert(module_ID,address_ID,attention,danger,flag,standard_water) values('$module_ID','$AD_ID','$attention','$danger',1,'$standard_water')";
            $update = "UPDATE alert set attention = '$attention',danger ='$danger',standard_water = '$standard_water' where module_ID = '$module_ID' and flag=1";

            if($judge==1){
                $stmt2 =$mysqli->query($update);
            }else{
                $stmt2 =$mysqli->query($insert);
            }
            echo "実行完了";
            echo "judge=".$judge;
        }
    }
    $url = "../complete_con.html?name=".$name;
    header("Location:" .$url);
}
?>