$(document).on("click", ".add", function() {
    var target = $(this).parent();
    if(target.parent().children().length < 5) 
    	$(this).parent().clone(true).insertAfter($(this).parent());
    else
	alert("一度に5つ以上のメールアドレスは設定できません。")

    
});
$(document).on("click", ".del", function() {
    var target = $(this).parent();
    if (target.parent().children().length > 1) {
        target.remove();
    }
});