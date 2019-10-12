$(document).on("click", ".add", function() {
    $(this).parent().before($(".input_plural").last().clone(true));
    $(".input_plural .form-control").last().val("");
  });
  $(document).on("click", ".del", function() {
      var target = $(this).parent();
      if (target.parent().children().length > 2) {
          target.remove();
      }
  });
  