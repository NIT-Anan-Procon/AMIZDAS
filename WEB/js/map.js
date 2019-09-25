//アイコン(図形)の追加
var icon1=L.icon({
  iconUrl: 'img/danger.png',
  iconSize:   [64,64],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

var icon2=L.icon({
  iconUrl: 'img/attention.png',
  iconSize:   [64,64],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

var icon3=L.icon({
  iconUrl: 'img/safe.png',
  iconSize:   [64,64],
  iconAnchor: [16,32],
  popupAnchor:[400,250]
});

window.onload=function(){
  //地図を表示するdiv要素のidを設定
  var map = L.map('mapcontainer');
  //地図の中心とズームレベルを指定
  map.setView([33.898777,134.667338], 11);
  //表示するタイルレイヤのURLとAttributionコントロールの記述を設定して、地図に追加する
  L.tileLayer('アトラス_2019-08-20_155246/4uMaps/{z}/{x}/{y}.png',{maxzoom: 15}).addTo(map);

   var sucontents = " <div id='popupsample1'><p id='title1' class='popup_title'>水位計情報</p> <br> <iframe src='http://localhost/amizdas/graph.html?num=2' class='gazo' frameborder='0'> </iframe><br><a id='searchLink' class='sample' href='http://localhost/amizdas/detail.html'> <button class='detail_button'>詳細な情報を確認する</button></a></div> ";
   //ポップアップオブジェクトを作成
   var popup1 = L.popup({ maxWidth: 625 }).setContent(sucontents);

   var sucontents2 = " <div id='popupsample2'><p id='title2' class='popup_title'>水位計情報</p> <br> <iframe src='http://localhost/amizdas/graph.html?num=1' class='gazo' frameborder='0'> </iframe> <br><a id='searchLink' class='sample' href='http://localhost/amizdas/detail.html'><button class='detail_button'>詳細な情報を確認する</button></a></div> ";
   //ポップアップオブジェクトを作成
   var popup2 = L.popup({ maxWidth: 625 }).setContent(sucontents2);


   const id_value=$('.popup_title').attr('id');
   //マーカーにポップアップを紐付けする。同時にbindTooltipでツールチップも追加
   L.marker([33.90,134.25], {icon: icon1}).addTo(map).bindPopup(popup1).bindTooltip("水位計n号機").on( 'click', function() {
   const id_value=$('.popup_title').attr('id');
   var target = document.getElementById("searchLink");
   target.href = "http://localhost/amizdas/detail.html?num=2";
});
   L.marker([34.05,134.50], {icon: icon1}).addTo(map).bindPopup(popup2).bindTooltip("水位計n号機").on( 'click', function() {
   const id_value=$('.popup_title').attr('id');
   var target = document.getElementById("searchLink");
   target.href = "http://localhost/amizdas/detail.html?num=1";
});
   L.marker([34.05,134.50], {icon: icon3}).addTo(map).bindPopup(popup1).bindTooltip("水位計1号機");
   L.marker([33.95,134.65], {icon: icon1}).addTo(map).bindPopup(popup2).bindTooltip("水位計n号機");
   L.marker([34.10,134.20], {icon: icon2}).addTo(map).bindPopup(popup2).bindTooltip("水位計n号機");
   L.marker([33.95,134.50], {icon: icon3}).addTo(map).bindPopup(popup2).bindTooltip("水位計n号機");
}
  /*function getId(ele){
    var id_value=ele.id;
    alert(id_value);
}*/



$(function () {
    //背景か閉じるボタンが押されたらポップアップを非表示
    
	//$('.gazo').click(function(){
	//	var a=$(this).attr('id');
	
	//)};
    $(".reload_button").on('click',function(){
       //リロード処理(仮)
       $(".popup").fadeOut();
       $(".popup").fadeIn();
    });
});

