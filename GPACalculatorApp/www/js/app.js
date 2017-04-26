// Ionic GPA Calculator App (Practice)
//Author: Shreyas Devalapurkar
//Date: August 16th, 2016

angular.module('starter', ['ionic'])

.controller ('TodoCtrl', function($scope, $ionicPopup, $ionicListDelegate){
    $scope.tasks = 
        [
            
        ];
    
    $scope.grades =
        [
        
        ];
    
    $scope.credits =
        [
        
        ];

    //initial function that allows you to create a new task when the compose icon is clicked
    $scope.newTask = function()
    {
        
        $ionicPopup.prompt({
            title: "Add Course",
            template: "Enter Course:",
            inputPlaceholder: "ex: Math 101",
            okText: "Submit",
        }).then(function(res){
            
            if(res == undefined)
            {
                return;
            }
            
            $ionicPopup.prompt({
                title: "Add Grade",
                template: "Enter Grade",
                inputPlaceholder: "ex: 9.0",
                okText: "Submit",
            }).then(function(res2){
                
                if(res2 == undefined)
                {
                    return;
                }
                
                if(isNaN(res2))
                {
                    $ionicPopup.alert({
                        title: "Please enter a number. Thank you."
                    })
                    return;
                }
                
                $ionicPopup.prompt({
                    title: "Add Credit Hours",
                    template: "Enter Credit Hours",
                    inputPlaceholder: "ex: 1.5",
                    okText: "Submit",
                }).then(function(res3){
                    
                    if(res3 == undefined)
                    {
                        return;
                    }
                    
                    if(res3 == "")
                    {
                        return;
                    }
                    
                    if(isNaN(res3))
                    {
                        $ionicPopup.alert({
                            title: "Please enter a number. Thank you."
                        })
                        return;
                    }
                        
                    if(res3 != undefined)
                    {      
                        if(res2 != undefined)
                        {
                            if(res)
                            {
                                $scope.grades.push(res2);
                                $scope.credits.push(res3);
                                $scope.tasks.push({title: res + " / Grade: " + res2 + " / Credit Hours: " + res3, grade: res2, credits: res3});
                            }
                        }   
                    }
                })
            })
        })
    };

    //function to allow you to reset the course details when edit button clicked
    $scope.editTask = function(task)
    {
            
        $ionicPopup.prompt({
            title: "Add Course",
            template: "Enter Course:",
            inputPlaceholder: "ex: Math 101",
            okText: "Submit",
        }).then(function(res){
            
            if(res == undefined)
            {
                return;
            }
            
            $ionicPopup.prompt({
                title: "Add Grade",
                template: "Enter Grade",
                inputPlaceholder: "ex: 9.0",
                okText: "Submit",
            }).then(function(res2){
                
                if(res2 == undefined)
                {
                    return;
                }
                
                $ionicPopup.prompt({
                    title: "Add Credit Hours",
                    template: "Enter Credit Hours",
                    inputPlaceholder: "ex: 1.5",
                    okText: "Submit",
                }).then(function(res3){
                    
                    if(res3 == undefined)
                    {
                        return;
                    }
                    
                    if(res3 == "")
                    {
                        return;
                    }
                    
                    if(res3 != undefined)
                    {    
                        if(res2 != undefined)
                        {
                            if(res)
                            {
                                var i = $scope.grades.indexOf(task.grade);
                                if(i != -1)
                                {
                                    $scope.grades.splice(i, 1);
                                }
                                
                                var j = $scope.credits.indexOf(task.credits);
                                if(j != -1)
                                {
                                    $scope.credits.splice(j, 1);
                                }
                                
                                $scope.grades.push(res2);
                                $scope.credits.push(res3);
                                $scope.tasks.splice($scope.tasks.indexOf(task), 1);
                                
                                $scope.tasks.push({title: res + " / Grade: " + res2 + " / Credit Hours: " + res3, grade: res2, credits: res3});
                                
                            }
                        }
                    }
                }) 
            })
        })
    };
    
    //function to delete a task, handles all the grades, credits, and tasks to be deleted specifically without messing up the GPA
    $scope.delete = function(task)
    {
        var x = $scope.tasks.indexOf(task);
        if(x != -1)
        {   
            $scope.tasks.splice(x, 1);
        }
        
        var i = $scope.grades.indexOf(task.grade);
        if(i != -1)
        {
            $scope.grades.splice(i, 1);
        }
                                
        var j = $scope.credits.indexOf(task.credits);
        if(j != -1)
        {
            $scope.credits.splice(j, 1);
        }
    }
    
    //function to calculate and display overall GPA when calculator icon on app is clicked
    $scope.showGPA = function()
    {
        
        if($scope.tasks.length == 0)
        {
            $ionicPopup.alert({
                title: "Current GPA" + " = 0.0",
            })
        }
        
        else
        {   
            var averageGPA;
            var sum = 0.0;   
            var credSum = 0.0;    
            
            for(var j = 0; j < $scope.grades.length; j++)
            {
                sum = sum + (parseFloat($scope.grades[j])*parseFloat($scope.credits[j]));
            }
            
            for(var k = 0; k < $scope.credits.length; k++)
            {
                credSum = credSum + parseFloat($scope.credits[k]);
            }
        
            averageGPA = ((sum)/(credSum));   
            var newAvg = averageGPA.toFixed(2);    
        
            var alertPopup = $ionicPopup.alert({
                title: "Current GPA" + " = " + newAvg,
            });
        }
    };
    
    //function to show info of a specific course when the course is clicked
    $scope.info = function(task)
    {
        
        $ionicPopup.alert({
            title: task.title,
            okText: "Okay",
        }) 
    };
    
})

.run(function($ionicPlatform) {
  $ionicPlatform.ready(function() {
    if(window.cordova && window.cordova.plugins.Keyboard) {
      // Hide the accessory bar by default (remove this to show the accessory bar above the keyboard
      // for form inputs)
      cordova.plugins.Keyboard.hideKeyboardAccessoryBar(true);

      // Don't remove this line unless you know what you are doing. It stops the viewport
      // from snapping when text inputs are focused. Ionic handles this internally for
      // a much nicer keyboard experience.
        
      cordova.plugins.Keyboard.disableScroll(true);
    }
    if(window.StatusBar) {
      StatusBar.styleDefault();
    }
  });
})
