from machine import Timer                    
timer = Timer('timer00')                            
                                               
    def callback_test(device):                  
        print("Timer callback test")
timer.init(timer.PERIODIC, 5000, callback_test) 
Timer callback_test     