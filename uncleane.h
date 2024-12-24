#ifndef UNCLEANE_H
#define UNCLEANE_H

void
SetCsmaAttributes(CsmaHelper& csma)
{
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
};

void
UCConfigSimulation(NodeContainer server_nodes,
                  NodeContainer switch_nodes,
                  NodeContainer router_nodes,
                  NodeContainer client_nodes)
{
    // ________________________ //
    // ===== Connect CSMA ===== //
    // ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ //
    /*
        Server
            Server ↔ Switch 1 ↔
                   ↔ Switch 1 ↔ Router ↔
    */
    CsmaHelper Server_Switch1_csma, Switch1_Router_csma;
    /*
        Router Switch
                              ↔ Router ↔ Switch 2 ↔
            Different Subnet
                              ↔ Router ↔ Switch 3 ↔
                              ↔ Router ↔ Switch 4 ↔
                              ↔ Router ↔ Switch 5 ↔
    */
    CsmaHelper Router_Switch2_csma, Router_Switch3_csma, Router_Switch4_csma, Router_Switch5_csma;
    /*
        Switch Client
                                       ↔ Switch 2 ↔ Client 1
            Same Subnet
                                       ↔ Switch 2 ↔ Client 2
                                       ↔ Switch 2 ↔ Client 3
                                       ↔ Switch 2 ↔ Client 4
            Different Subnet
                                       ↔ Switch 3 ↔ Client 2
                                       ↔ Switch 4 ↔ Client 3
                                       ↔ Switch 5 ↔ Client 4
    */
    CsmaHelper Switch2_Client1_csma;
    CsmaHelper Switch2_Client2_csma, Switch2_Client3_csma, Switch2_Client4_csma;
    CsmaHelper Switch3_Client2_csma, Switch4_Client3_csma, Switch5_Client4_csma;

    // ______________________________ //
    // ===== Set CSMA Attribute ===== //
    // ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ //
    SetCsmaAttributes(Server_Switch1_csma);
    SetCsmaAttributes(Switch1_Router_csma);
    if (g_subnet == "same")
    {
        // Router to Switches
        SetCsmaAttributes(Router_Switch2_csma);
        // Switches to Clients
        SetCsmaAttributes(Switch2_Client1_csma);
        SetCsmaAttributes(Switch2_Client2_csma);
        SetCsmaAttributes(Switch2_Client3_csma);
        SetCsmaAttributes(Switch2_Client4_csma);
    }
    else
    {
        // Router to Switches
        SetCsmaAttributes(Router_Switch2_csma);
        SetCsmaAttributes(Router_Switch3_csma);
        SetCsmaAttributes(Router_Switch4_csma);
        SetCsmaAttributes(Router_Switch5_csma);
        // Switches to Clients
        SetCsmaAttributes(Switch2_Client1_csma);
        SetCsmaAttributes(Switch3_Client2_csma);
        SetCsmaAttributes(Switch4_Client3_csma);
        SetCsmaAttributes(Switch5_Client4_csma);
    }

    // Server
    NetDeviceContainer Server_Switch1_devices, Switch1_Router_devices;

    // Same Subnet
    NetDeviceContainer Router_Switch2_devices;
    NetDeviceContainer Switch2_Client1_devices, Switch2_Client2_devices, Switch2_Client3_devices,
        Switch2_Client4_devices;

    // Difference Subnet
    NetDeviceContainer Router_Switch3_devices, Router_Switch4_devices, Router_Switch5_devices;
    NetDeviceContainer Switch3_Client2_devices, Switch4_Client3_devices, Switch5_Client4_devices;

    // Server
    Server_Switch1_devices =
        Server_Switch1_csma.Install(NodeContainer(server_nodes.Get(0), switch_nodes.Get(0)));
    Switch1_Router_devices =
        Switch1_Router_csma.Install(NodeContainer(switch_nodes.Get(0), router_nodes.Get(0)));

    if (g_subnet == "same")
    { // Same Subnet
        // Router to Switches
        Router_Switch2_devices =
            Router_Switch2_csma.Install(NodeContainer(router_nodes.Get(0), switch_nodes.Get(1)));
        // Switches to Clients
        Switch2_Client1_devices =
            Switch2_Client1_csma.Install(NodeContainer(switch_nodes.Get(1), client_nodes.Get(0)));
        Switch2_Client2_devices =
            Switch2_Client2_csma.Install(NodeContainer(switch_nodes.Get(1), client_nodes.Get(1)));
        Switch2_Client3_devices =
            Switch2_Client3_csma.Install(NodeContainer(switch_nodes.Get(1), client_nodes.Get(2)));
        Switch2_Client4_devices =
            Switch2_Client4_csma.Install(NodeContainer(switch_nodes.Get(1), client_nodes.Get(3)));
    }
    else
    { // Difference Subnet
        // Router to Switches
        Router_Switch2_devices =
            Router_Switch2_csma.Install(NodeContainer(router_nodes.Get(0), switch_nodes.Get(1)));
        Router_Switch3_devices =
            Router_Switch3_csma.Install(NodeContainer(router_nodes.Get(0), switch_nodes.Get(2)));
        Router_Switch4_devices =
            Router_Switch4_csma.Install(NodeContainer(router_nodes.Get(0), switch_nodes.Get(3)));
        Router_Switch5_devices =
            Router_Switch5_csma.Install(NodeContainer(router_nodes.Get(0), switch_nodes.Get(4)));
        // Switches to Clients
        Switch2_Client1_devices =
            Switch2_Client1_csma.Install(NodeContainer(switch_nodes.Get(1), client_nodes.Get(0)));
        Switch3_Client2_devices =
            Switch3_Client2_csma.Install(NodeContainer(switch_nodes.Get(2), client_nodes.Get(1)));
        Switch4_Client3_devices =
            Switch4_Client3_csma.Install(NodeContainer(switch_nodes.Get(3), client_nodes.Get(2)));
        Switch5_Client4_devices =
            Switch5_Client4_csma.Install(NodeContainer(switch_nodes.Get(4), client_nodes.Get(3)));
    }

    InternetStackHelper internet;
    internet.InstallAll();

    // ======================================================= //
    // ==================== Bridge Switch ==================== //
    // ======================================================= //
    /*
        Switch 1
            Server ↔ Switch 1 ↔
                   ↔ Switch 1 ↔ Router ↔
    */
    NetDeviceContainer Switch1_Devices;
    Switch1_Devices.Add(Server_Switch1_devices.Get(1));
    Switch1_Devices.Add(Switch1_Router_devices.Get(0));
    /*
        Swtich 2
                              ↔ Router ↔ Switch 2 ↔
                                       ↔ Switch 2 ↔ Client 1
                                       ↔ Switch 2 ↔ Client 2
                                       ↔ Switch 2 ↔ Client 3
                                       ↔ Switch 2 ↔ Client 4
        Switch 3
                              ↔ Router ↔ Switch 3 ↔
                                       ↔ Switch 3 ↔ Client 2
        Switch 4
                              ↔ Router ↔ Switch 4 ↔
                                       ↔ Switch 4 ↔ Client 3
        Switch 5
                              ↔ Router ↔ Switch 5 ↔
                                       ↔ Switch 5 ↔ Client 4
    */
    NetDeviceContainer Switch2_Devices, Switch3_Devices, Switch4_Devices, Switch5_Devices;
    Switch2_Devices.Add(Router_Switch2_devices.Get(1));
    Switch2_Devices.Add(Switch2_Client1_devices.Get(0));
    if (g_subnet == "same")
    { // Same Subnet
        Switch2_Devices.Add(Switch2_Client2_devices.Get(0));
        Switch2_Devices.Add(Switch2_Client3_devices.Get(0));
        Switch2_Devices.Add(Switch2_Client4_devices.Get(0));
    }
    else
    { // Difference Subnet
        Switch3_Devices.Add(Router_Switch3_devices.Get(1));
        Switch3_Devices.Add(Switch3_Client2_devices.Get(0));
        Switch4_Devices.Add(Router_Switch4_devices.Get(1));
        Switch4_Devices.Add(Switch4_Client3_devices.Get(0));
        Switch5_Devices.Add(Router_Switch5_devices.Get(1));
        Switch5_Devices.Add(Switch5_Client4_devices.Get(0));
    }

    BridgeHelper bridge;
    Ptr<Node> switch1_bridge_node = switch_nodes.Get(0);
    bridge.Install(switch1_bridge_node, Switch1_Devices);

    Ptr<Node> switch2_bridge_node = switch_nodes.Get(1);
    bridge.Install(switch2_bridge_node, Switch2_Devices);

    if (g_subnet != "same")
    {
        Ptr<Node> switch3_bridge_node = switch_nodes.Get(2);
        bridge.Install(switch3_bridge_node, Switch3_Devices);

        Ptr<Node> switch4_bridge_node = switch_nodes.Get(3);
        bridge.Install(switch4_bridge_node, Switch4_Devices);

        Ptr<Node> switch5_bridge_node = switch_nodes.Get(4);
        bridge.Install(switch5_bridge_node, Switch5_Devices);
    }

    // =================================================== //
    // ==================== Assign IP ==================== //
    // =================================================== //
    Ipv4AddressHelper address;
    // Server Subnet
    Ipv4InterfaceContainer Server_Switch1_interfaces, Switch1_Router_interfaces;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Server_Switch1_interfaces = address.Assign(Server_Switch1_devices.Get(0)); // Assign Server IP
    Switch1_Router_interfaces = address.Assign(Switch1_Router_devices.Get(1)); // Assign Router IP

    // Client Subnet
    Ipv4InterfaceContainer Router_Switch2_interfaces;
    Ipv4InterfaceContainer Switch2_Client1_interfaces, Switch2_Client2_interfaces,
        Switch2_Client3_interfaces, Switch2_Client4_interfaces;
    Ipv4InterfaceContainer Router_Switch3_interfaces, Router_Switch4_interfaces,
        Router_Switch5_interfaces;
    Ipv4InterfaceContainer Switch3_Client2_interfaces, Switch4_Client3_interfaces,
        Switch5_Client4_interfaces;
    address.SetBase("192.168.2.0", "255.255.255.0");
    Router_Switch2_interfaces = address.Assign(Router_Switch2_devices.Get(0)); // Assign Router IP
    Switch2_Client1_interfaces =
        address.Assign(Switch2_Client1_devices.Get(1)); // Assign Client 1 IP
    if (g_subnet == "same")
    { // Same Subnet
        Switch2_Client2_interfaces =
            address.Assign(Switch2_Client2_devices.Get(1)); // Assign Client 2 IP
        Switch2_Client3_interfaces =
            address.Assign(Switch2_Client3_devices.Get(1)); // Assign Client 3 IP
        Switch2_Client4_interfaces =
            address.Assign(Switch2_Client4_devices.Get(1)); // Assign Client 4 IP
    }
    else
    { // Difference Subnet
        address.SetBase("192.168.3.0", "255.255.255.0");
        Router_Switch3_interfaces =
            address.Assign(Router_Switch3_devices.Get(0)); // Assign Router IP
        Switch3_Client2_interfaces =
            address.Assign(Switch3_Client2_devices.Get(1)); // Assign Client 2 IP
        address.SetBase("192.168.4.0", "255.255.255.0");
        Router_Switch4_interfaces =
            address.Assign(Router_Switch4_devices.Get(0)); // Assign Router IP
        Switch4_Client3_interfaces =
            address.Assign(Switch4_Client3_devices.Get(1)); // Assign Client 3 IP
        address.SetBase("192.168.5.0", "255.255.255.0");
        Router_Switch5_interfaces =
            address.Assign(Router_Switch5_devices.Get(0)); // Assign Router IP
        Switch5_Client4_interfaces =
            address.Assign(Switch5_Client4_devices.Get(1)); // Assign Client 4 IP
    }

    Ptr<Ipv4> ipv4 = router_nodes.Get(0)->GetObject<Ipv4>();
    ipv4->SetAttribute("IpForward", BooleanValue(true));
}

#endif // UNCLEANE_H