<?PHP
    //sql接続
    require_once('../Config/SQLServer.php');
    $mysqli = MySQLi();
    if($mysqli->connect_errno){
    echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
    }
    mysqli_set_charset( $mysqli, 'utf8');

    $file = ["csvfile.csv","csvfile_sakura.csv","csvfile1.csv","csvfile2.csv","csvfile_SS001.csv"];
    $module = ["739D7D","uGqZClHAVHB2","739FFC","73E88A","uoTrJtFgSaqB"];
    $module_name = ["水位計一号機","水位計二号機","水位計三号機","水位計四号機","水位計五号機"];

    for($i=0;$i<count($module);$i++){
        $lines = file($file[$i], FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        $water = preg_split("/[、,]/",$lines[count($lines)-1]);
        $sql = "UPDATE water_level SET 
                time = '$water[0]',
                water_gage = $water[1],
                radiation = $water[2],
                vattery_voltage = $water[3],
                temperature = $water[4],
                water_gage2 = $water[5],
                lat = $water[6],
                lng = $water[7]
                where module_ID = '$module[$i]'";
    
    $stmt = $mysqli->query($sql);
    }
    $mysqli->close();
?>

