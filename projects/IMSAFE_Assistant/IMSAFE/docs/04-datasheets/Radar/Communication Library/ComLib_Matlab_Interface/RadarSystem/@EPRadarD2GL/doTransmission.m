function doTransmission(obj, message, poll_response, optionalparameter)

if nargin == 2
  poll_rsp = 1;  
  wait = 0;
elseif nargin == 3
  poll_rsp = poll_response;
  wait = 0;
elseif nargin == 4
  poll_rsp = poll_response;
  wait = optionalparameter;
else
  wait = 0;
end


% send message
obj.oRS.sendMessage(obj.uEndpointID, message)

for i=1:poll_rsp
    % get status message
    if( wait > 0) 
        pause( wait ); 
    end
    obj.oRS.processResponse;
end