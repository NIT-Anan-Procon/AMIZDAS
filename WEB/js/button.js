$('#AjaxForm').submit(function(event){
        event.preventDefault();
        var $form = $(this);
        var $button = $form.find('.submit');
        $.ajax({
            url: $form.attr('action'),
            type: $form.attr('method'),
            data: $form.serialize(),
            timeout: 100000,

            beforeSend: function(xhr, settings){
                $button.attr('disabled', true);
            
$("#result").text("")
        },

        complete: function(result, textStatus){
            event.preventDefault();
            $button.attr('disabled', false);
        }
         });
});