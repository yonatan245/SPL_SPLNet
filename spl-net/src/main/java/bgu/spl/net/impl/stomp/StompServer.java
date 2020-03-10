package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Server;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Supplier;

public class StompServer {

    public static void main(String[] args) {

        if(args.length < 2) System.out.println("Illegal input!");
        else{

            int port = Integer.parseInt(args[0]);
            String serverType = args[1];

            Supplier<StompMessagingProtocol> protocolFactory = new Supplier<StompMessagingProtocol>() {
                private AtomicInteger connectionID = new AtomicInteger(1);
                private Connections connections = StompConnections.getInstance();

                @Override
                public StompMessagingProtocolImpl get() {
                    StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
                    protocol.start(connectionID.getAndIncrement(), connections);

                    return protocol;
                }
            };

            if(serverType.compareTo("tpc") == 0)
                Server.threadPerClient(port,
                        protocolFactory,
                        ()-> new StompMessageEncoderDecoder()).serve();

            else if(serverType.compareTo("reactor")==0) Server.reactor(4, port,
                    protocolFactory,
                    ()-> new StompMessageEncoderDecoder()).serve();
        }
    }
}

