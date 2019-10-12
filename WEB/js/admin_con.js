$(window).load(function() {
  $('#message').submit(function( event ) {
    event.preventDefault();

    $.post('../phpfile/send_mail.php', $('#message').serialize())
    //サーバーからの返信を受け取る
    .done(function(data){
    })

    //通信エラーの場合
    .fail(function(){
    })

    //通信が終了した場合
    .always(function(){
    })
  })
});
