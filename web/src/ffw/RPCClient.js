/*
 * Base class for RPC client
 * 
 * Class provides access to basic functionality of
 * RPC components Message Broker as: 
 * 		registerComponent
 * 		unregisterComponent
 * 		subscription to notifications
 * 		logic to calculate request id
 * 
 * @author Sergey Solovey
 */

FFW.RPCClient = Em.Object.extend({
		
	/*
     * transport layer for messages exchange
 	 */		
	socket: 				null, 			// instance of websocket

	/*
     * url for message broker
 	 */		
	url: 					"ws://localhost:8086",
//	url: 					"ws://echo.websocket.org/", 	

	/*
     * Component name in RPC system
	 * It is unique.
 	 */		
	componentName:			null,
	
	/*
     * observer of RPC states
 	 */		
	observer: 				null,

	/*
     * these variables are used to have unique request ids for different components in RPC bus
	 * idStart is received as a response for registerRPCComponent messages.
	 * space for ids for specific RPC component is allocated by message broker 
 	 */		
	idStart: 				-1,
	idRange: 				1000,
	requestId:				-1,

	registerRequestId: 		-1,
	unregisterRequestId: 	-1,

	/*
     * Open WebSocket and initialize handlers
 	 */
	connect: function(observer) {
		this.observer			= observer;
		//for French version use nother port
		if(FLAGS.locale == "fr"){
			this.set('url',"ws://localhost:8088");
		} else if(FLAGS.locale == "spa") {
			this.set('url',"ws://localhost:8089");
		}

		this.socket 			= (FFW.isAndroid || (FLAGS.EMULATE_ANDROID && FLAGS.EMULATE_WS)) ?  FFW.WebSocket.create({clientName: this.componentName}) : new WebSocket(this.url, 'sample');
      
		var self = this;

		this.socket.onopen 		= function(evt) { self.onWSOpen(evt) };
		this.socket.onclose 	= function(evt) { self.onWSClose(evt) };
		this.socket.onmessage 	= function(evt) { self.onWSMessage(evt) };
		this.socket.onerror 	= function(evt) { self.onWSError(evt) };
		
		// only for Android!!! emulate that connection is openned
		if (FFW.isAndroid || (FLAGS.EMULATE_ANDROID && FLAGS.EMULATE_WS))
		{
			this.socket.onopen(null);
		}
	},

	/*
     * Close WebSocket connection
	 * Please make sure that component was unregistered in advance
 	 */	
	disconnect: function() {
		this.unregisterRPCComponent();
	},

	/*
     * WebSocket connection is ready
	 * Now RPC component can be registered in message broker
 	 */	
	onWSOpen: function (evt) {
		Em.Logger.log("RPCCLient.onWSOpen");
		
		this.registerRPCComponent();
	},

	/*
     * when result is received from RPC component this function is called
	 * It is the propriate place to check results of reuqest execution
	 * Please use previously store reuqestID to determine to which request repsonse belongs to
 	 */	
	onWSMessage: function (evt) {
		Em.Logger.log("Message received: " + evt.data);
		
		var jsonObj = JSON.parse(evt.data);
		
		// handle component registration
		if (jsonObj.id == this.registerRequestId) {
			if (jsonObj.error == null) {
				this.requestId = this.idStart = jsonObj.result;
				this.observer.onRPCRegistered();
			}
		// handle component unregistration
		} else if (jsonObj.id == this.unregisterRequestId) {
			if (jsonObj.error == null) {
				this.socket.close();
				this.observer.onRPCUnregistered();
			}
		// handle result, error, notification, requests
		} else {
			if (jsonObj.id == null) {
				this.observer.onRPCNotification(jsonObj);
			} else {
				if (jsonObj.result 	!= null)
					this.observer.onRPCResult(jsonObj);
				else if (jsonObj.error 	!= null)
					this.observer.onRPCError(jsonObj);
				else
					this.observer.onRPCRequest(jsonObj);				
			}
		}		
	},
	
	/*
     * WebSocket connection is closed
	 * Please make sure that RPCComponent was dunregistered in advance
 	 */	
	 onWSClose: function (evt) {
		Em.Logger.log("RPCClient: Connection is closed");
		this.observer.onRPCDisconnected();
	},
  	
	/*
     * WebSocket connection errors handling
	 */
	onWSError: function (evt) {
		//Em.Logger.log("ERROR: " + evt.data);
		Em.Logger.log("ERROR: ");
	},

	/*
     * register component is RPC bus
	 */
	registerRPCComponent: function() {
		this.registerRequestId = 200;

		var JSONMessage = {
			"jsonrpc":	"2.0",
			"id": 		this.registerRequestId,
			"method":	"MB.registerComponent",
			"params":	{
				"componentName": this.componentName
			}
		};
		this.send(JSONMessage);
	},

	/*
     * unregister component is RPC bus
	 */
	unregisterRPCComponent: function() {
		this.unregisterRequestId = this.generateId();

		var JSONMessage = {
			"jsonrpc":	"2.0",
			"id": 		this.unregisterRequestId,
			"method":	"MB.unregisterComponent",
			"params":	{
				"componentName": this.componentName
			}
		};
		this.send(JSONMessage);
	},
	
	/*
     * stringify object and send via socket connection
 	 */	
	send: function(obj) {
		var strJson = JSON.stringify(obj);
		Em.Logger.log(strJson);
		this.socket.send(strJson);
	},
	
	/*
     * Generate id for new request to RPC component
	 * Function has to be used as private
 	 */	
	generateId: function() {
		this.requestId++;
		if (this.requestId >= this.idStart + this.idRange)
			this.requestId = this.idStart;
		return this.requestId;
	}
});
