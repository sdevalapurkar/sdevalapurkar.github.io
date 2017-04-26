<?php
    if($_REQUEST[job] == "send_email")
    {
        
        $name = $_POST['name'];
        $email = $_POST['email'];
        $message = $_POST['text'];
        
        if (($name=="")||($email=="")||($message==""))
        {
            echo "All fields are required, please fill <a href=\"\">the form</a> again.";
	    }
        else
        {		
            $from="From: $name<$email>\r\nReturn-path: $email";
            $subject="Message sent using your contact form";
            mail("shreyasdevalapurkar@gmail.com", $subject, $message, $from);
            echo "Your message has been sent!";
	    }
        
    }  
        
        //$msg=$_POST["name"]."\n".$_POST["phone"]."\n".$_POST["text"];
        //mail("shreyasdevalapurkar@gmail.com", $_POST["subject"], $msg, "From: ".$_POST["email"]."\r\n");
        /*
        send_mail($_REQUEST[name], $_REQUEST[email], $_REQUEST[subject], $_REQUEST[text]);
    }

    function send_mail($name, $email, $subject, $text){
        $myname = "Shreyas Devalapurkar";
        $myemail = "shreyasdevalapurkar@gmail.com";
        
        $useremail = $name."(".$email.")";
        $toemail = $myname."(".$myemail.")";
        mail($toemail, $subject, $text, "From:".$useremail);
        
        echo "Your message has been sent";
    }
    */
?>

        