//アイコン(図形)の追加
var icon1=L.icon({
  iconUrl: 'img/danger.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

var icon2=L.icon({
  iconUrl: 'img/attention.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

var icon3=L.icon({
  iconUrl: 'img/safe.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

var icon_finally;

window.onload=function(){
  
  //地図を表示するdiv要素のidを設定
  var map = L.map('mapcontainer');
  
  var $script1 = $('#script');
  var water_gage_data = JSON.parse($script1.attr('data-param1'));

  var $script2 = $('#script');
  var danger_data = JSON.parse($script2.attr('data-param2'));

  var $script3 = $('#script');
  var attention_data = JSON.parse($script3.attr('data-param3'));

  var $script4 = $('#script');
  var lat_data = JSON.parse($script4.attr('data-param4'));

  var $script5 = $('#script');
  var lng_data = JSON.parse($script5.attr('data-param5'));

  var $script6 = $('#script');
  var array_count = JSON.parse($script6.attr('data-param6'));

  var $script7 = $('#script');
  var prefecture = JSON.parse($script7.attr('data-param7'));

  //地図の中心とズームレベルを指定
  if(prefecture=='tokushima'){
  	map.setView([33.898777,134.667338], 11);
  }
  else if(prefecture=='tokyo'){
  	map.setView([35.660253120093344,139.69267273787412], 11);
  }
  else if(prefecture=='miyazaki'){
       	map.setView([31.80000,131.32164003560322], 10);
  }
  else{
  map.setView([33.898777,134.667338], 11);
  }

  //表示するタイルレイヤのURLとAttributionコントロールの記述を設定して、地図に追加する
  L.tileLayer('MapData/4uMaps/{z}/{x}/{y}.png',{maxzoom: 15}).addTo(map);
   var sucontents = " <div id='popupsample1'><p id='title1' class='popup_title'>水位計情報</p> <br> <iframe src='graph.html?name=水位計二号機' class='gazo' frameborder='0'> </iframe><br><a id='searchLink' class='sample' href='detail.html?name=水位計二号機'> <button class='detail_button'>詳細な情報を確認する</button></a><a href='alert_con.html?name=水位計二号機'><button class='alert_button' >アラートを設定する</button></a></div> ";
   //ポップアップオブジェクトを作成
   var popup1 = L.popup({ maxWidth: 800 }).setContent(sucontents);

  var sucontents2 = " <div id='popupsample2'><p id='title2' class='popup_title'>水位計情報</p> <br> <iframe src='graph.html?name=水位計一号機' class='gazo' frameborder='0'> </iframe> <br><a id='searchLink' class='sample' href='detail.html?name=水位計一号機'><button class='detail_button'>詳細な情報を確認する</button></a><a href='alert_con.html?name=水位計一号機'><button class='alert_button' >アラートを設定する</button></a></div> ";
   //ポップアップオブジェクトを作成
   var popup2 = L.popup({ maxWidth: 800 }).setContent(sucontents2);
    const id_value=$('.popup_title').attr('id');

  var i=0;
  
  if(Math.floor(water_gage_data[i])>danger_data[i]){
	 icon_finally=icon1;
  }else if(Math.floor(water_gage_data[i])<danger_data[i] & Math.floor(water_gage_data[i])>attention_data[i]){
	 icon_finally=icon2;
  }else{
	 icon_finally=icon3;
  }

  L.marker([lng_data[i],lat_data[i]], {icon: icon_finally}).addTo(map).bindPopup(popup1).bindTooltip("水位計1号機").on( 'click', function() {
   const id_value=$('.popup_title').attr('id');
   var target = document.getElementById("searchLink");
   target.href = "detail.html?name=水位計二号機";
});
  i++;

  if(Math.floor(water_gage_data[i])>danger_data[i]){
	 icon_finally=icon1;
  }else if(Math.floor(water_gage_data[i])<danger_data[i] & Math.floor(water_gage_data[i])>attention_data[i]){
	 icon_finally=icon2;
  }else{
	 icon_finally=icon3;
  }
  L.marker([lng_data[i],lat_data[i]], {icon: icon_finally}).addTo(map).bindPopup(popup2).bindTooltip("水位計2号機").on( 'click', function() {
   const id_value=$('.popup_title').attr('id');
   var target = document.getElementById("searchLink");
   target.href = "detail.html?name=水位計一号機";
});

   L.marker([34.10,134.55], {icon: icon1}).addTo(map).bindPopup(popup2);
   L.marker([33.95,134.65], {icon: icon1}).addTo(map).bindPopup(popup2);
   L.marker([34.10,134.20], {icon: icon2}).addTo(map).bindPopup(popup2);
   L.marker([33.98,134.53], {icon: icon3}).addTo(map).bindPopup(popup2);
   L.marker([33.75,134.55], {icon: icon2}).addTo(map).bindPopup(popup2);
   L.marker([34.08,134.35], {icon: icon2}).addTo(map).bindPopup(popup2);
   L.marker([35.6812405,139.7649361], {icon: icon3}).addTo(map).bindPopup(popup2);//東京駅に配置
   L.marker([31.9109129,131.4217089], {icon: icon3}).addTo(map).bindPopup(popup2);//宮崎県庁に配置
}

