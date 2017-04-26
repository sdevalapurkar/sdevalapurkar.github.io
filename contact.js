var $contactForm = $('#contact-form');
$contactForm.submit(function(e) {
    
    var nameField = document.getElementById("name");
    var emailField = document.getElementById("email");
    var subjectField = document.getElementById("subject");
    var textField = document.getElementById("message");
    
    if(!nameField.value)
    {
        alert("Please enter a name");
        nameField.focus();
        return false;
    }
    
    if(!emailField.value)
    {
        alert("Please enter an email address");
        emailField.focus();
        return false;
    }
    
    if(!subjectField.value)
    {
        alert("Please enter a subject");
        subjectField.focus();
        return false;
    }
    
    if(!textField.value)
    {
        alert("Please enter a message");
        textField.focus();
        return false;
    }
    
    e.preventDefault();
    $.ajax({
        url: '//formspree.io/shreyasdevalapurkar@gmail.com',
        method: 'POST',
        data: $(this).serialize(),
        dataType: 'json',
        beforeSend: function() {
            /* $contactForm.append('<div class="w-form-pending"><p> Sending... </p></div>'); */
        },
        success: function(data) {
            alert("Thanks for your message. I will get back to you as soon as possible.");
            //$contactForm.append('<div class="w-form-done"><p class="text-center"> Thanks for Contacting Kitchen King. Our sales consultant will contact you as soon as possible.</p></div>');
        },
        error: function(err) {
            alert("Oops! Something went wrong while sending your message. Please try again later.");
            //$contactForm.append('<div class="w-form-done"><p class="text-center"> Oops! Something went wrong while submitting the form, Please  send us an email directly on sales@kitchenking.ae</p></div>');
            $contactForm.find('.w-form-pending').hide();
            
        }
    });
});


