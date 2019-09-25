$(function() {
  //モーダルを開く
  $('.set_button').click(function() {
    $('.overlay, .modal').fadeIn();
  })
  //モーダルの外側か閉じるをクリックでモーダルを閉じる
  $('.overlay, .close').click(function() {
    $('.overlay, .modal').fadeOut();
  })
});