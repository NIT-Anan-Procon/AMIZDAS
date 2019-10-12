$(function() {
  //���[�_�����J��
  $('.modalBtn').click(function() {
    $('.overlay, .modal').fadeIn();
  })
  //���[�_���̊O����������N���b�N�Ń��[�_�������
  $('.overlay, .close').click(function() {
    $('.overlay, .modal').fadeOut();
  })
});
