import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Random;


public class Create_xml_files {
	
	
	public static void create_xml_deployement(Connection conn, int nb_workers, int nb_clients, int nb_primaries, String directory) {
		FileWriter fw = null;
		
		try {
			// create the header
			if (nb_primaries == 1) {
				fw = new FileWriter (directory + "/deployement_centralized_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			}
			else {
				fw = new FileWriter (directory + "/deployement_distributed_" + nb_primaries + "primaries_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			}
			fw.write("<?xml version='1.0'?>\n");
			fw.write("<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n");
			fw.write("<platform version=\"3\">\n\n");
		
			
			if (nb_primaries > 1) {
				// indicate parameters for first-primary
				fw.write("\t<process host=\"first-primary0\" function=\"first_primary\">\n");
				fw.write("\t\t<argument value=\"0\"/> <!-- id of the first-primary -->\n");
				fw.write("\t\t<argument value=\"" + nb_clients + "\"/> <!-- number of clients -->\n");
				fw.write("\t</process>\n");
				
				fw.write("\n");
				fw.flush();
				
				
				// indicate parameters for primary
				for (int i = 0; i < nb_primaries; i++) {
					fw.write("\t<process host=\"primary" + i + "\" function=\"primary\">\n");
					fw.write("\t\t<argument value=\"" + i + "\"/> <!-- id of the primary -->\n");
					fw.write("\t\t<argument value=\"first-primary-0\"/> <!-- name of the first-primary -->\n");
					fw.write("\t</process>\n");
				}				
			}
			else {
				// indicate parameters for primary
				fw.write("\t<process host=\"primary0\" function=\"primary\">\n");
				fw.write("\t\t<argument value=\"0\"/> <!-- id of the primary -->\n");
				fw.write("\t\t<argument value=\"" + nb_clients + "\"/> <!-- number of clients -->\n");
				fw.write("\t</process>\n");
			}
			
			fw.write("\n");
			fw.flush();
			
			// indicate parameters for clients
			for (int i = 0; i < nb_clients; i++) {
				fw.write("\t<process host=\"client" + i + "\" function=\"client\">\n");
				fw.write("\t\t<argument value=\"" + i + "\"/> <!-- id of the client -->\n");
				if (nb_primaries == 1) {
					fw.write("\t\t<argument value=\"primary-0\"/> <!-- name of the primary -->\n");
				}
				else {
					fw.write("\t\t<argument value=\"first-primary-0\"/> <!-- name of the first-primary -->\n");
				}
				fw.write("\t</process>\n");
			}
			
			fw.write("\n");
			fw.flush();
		
			// indicate parameters for workers
			Statement stmt;
			ResultSet rs;
			try {
				int i = 0;
				stmt = conn.createStatement();
				rs = stmt.executeQuery("select node_id from event_trace group by node_id order by node_id limit " + nb_workers);
				while(rs.next()){
					fw.write("\t<process host=\"worker" + i + "\" function=\"worker\">\n");
					fw.write("\t\t<argument value=\"" + rs.getInt(1) + "\"/> <!-- id of the worker -->\n");
					fw.write("\t\t<argument value=\"" + i + "\"/> <!-- number of the workers used for the global table -->\n");
					if (nb_primaries == 1) {
						fw.write("\t\t<argument value=\"primary-0\"/> <!-- name of the primary -->\n");
					}
					else {
						fw.write("\t\t<argument value=\"first-primary-0\"/> <!-- name of the first-primary -->\n");
					}
					int reputation = new Random().nextInt(100);
					if (reputation < 30) {
						reputation = -1;
					}
					else if (reputation < 50) {
						reputation = 0;
					}
					else {
						reputation = 1;
					}
					
					fw.write("\t\t<argument value=\"" + reputation + "\"/> <!-- value of the reputation -->\n");
					fw.write("\t</process>\n");
					i++;
				}
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			// end of the xml file
			fw.write("</platform>\n");
			fw.flush();
		
		} catch (IOException e2) {
			e2.printStackTrace();
		}
		
	}
	
	public static void create_xml_platform(Connection conn, int nb_workers, int nb_clients, int nb_primaries, String directory) {
		FileWriter fw = null;
		int nb_route = 0;
		
		try {
			// create the new file
			if (nb_primaries == 1) {
				fw = new FileWriter (directory + "/platform_centralized_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			}
			else {
				fw = new FileWriter (directory + "/platform_distributed_" + nb_primaries + "primaries_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			}
			// write the header
			fw.write("<?xml version='1.0'?>\n");
			fw.write("<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n");
			fw.write("<platform version=\"3\">\n\n");
			
			fw.write("\t<config id=\"general\">\n");
			fw.write("\t\t <prop id=\"network/model\" value=\"LV08\"/>\n");
			fw.write("\t</config>\n");
			
			fw.write("\t<AS id=\"AS0\" routing=\"Full\">\n\n");
			
			// create the hosts of the system
			if (nb_primaries > 1) {
				fw.write("\t\t<host id=\"first-primary0\" power=\"2000000000\"/>\n\n");
				for (int i = 0; i < nb_primaries; i++) {
					fw.write("\t\t<host id=\"primary" + i + "\" power=\"2000000000\"/>\n\n");
				}
			}
			else {
				fw.write("\t\t<host id=\"primary0\" power=\"2000000000\"/>\n\n");
			}
		
			fw.flush();
			
			for (int i = 0; i < nb_clients; i++) {
				fw.write("\t\t<host id=\"client" + i + "\" power=\"2000000000\"/>\n");
			}
			fw.write("\n");
			for (int i = 0; i < nb_workers; i++) {
				fw.write("\t\t<host id=\"worker" + i +"\" power=\"2000000000\"/>\n");
			}
			fw.write("\n");
			
			fw.flush();
			
			if (nb_primaries == 1) {
				// create the links of the system
				for (int i = 0; i < nb_clients + nb_workers; i++) {
					fw.write("\t\t<link id=\"link" + i +"\" bandwidth=\"7.24MBps\" latency=\"0.004\"/>\n");
				}
			}
			else {
				// create the links of the system
				for (int i = 0; i < nb_clients + nb_workers + nb_primaries + nb_clients * nb_primaries + nb_workers * nb_primaries + ((nb_primaries * (nb_primaries - 1)) / 2); i++) {
					fw.write("\t\t<link id=\"link" + i +"\" bandwidth=\"7.24MBps\" latency=\"0.004\"/>\n");
				}
			}
			fw.write("\n");
			
			if (nb_primaries == 1) {
				// create the routes of the system (to client towards primary)
				for (int i = 0; i < nb_clients; i++) {
					fw.write("\t\t<route src=\"client" + i + "\" dst=\"primary0\" symmetrical=\"YES\">\n");
					fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
					fw.write("\t\t</route>\n");
					
					nb_route++;
				}
				
				// create the routes of the system (to primary to workers)
				for (int i = 0; i < nb_workers; i++) {
					fw.write("\t\t<route src=\"worker" + i + "\" dst=\"primary0\" symmetrical=\"YES\">\n");
					fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
					fw.write("\t\t</route>\n");
					
					nb_route++;
				}				
			}
			else {
				// create the routes of the system (to client towards first-primary)
				for (int i = 0; i < nb_clients; i++) {
					fw.write("\t\t<route src=\"client" + i + "\" dst=\"first-primary0\" symmetrical=\"YES\">\n");
					fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
					fw.write("\t\t</route>\n");
					
					nb_route++;
				}
				fw.write("\n");
				fw.flush();
				
				// create the routes of the system (to first-primary towards primaries)
				for (int i = 0; i < nb_primaries; i++) {
					fw.write("\t\t<route src=\"primary" + i + "\" dst=\"first-primary0\" symmetrical=\"YES\">\n");
					fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
					fw.write("\t\t</route>\n");
					
					nb_route++;
				}
				fw.write("\n");
				fw.flush();
				
				// create the routes of the system (to first-primary towards workers)
				for (int i = 0; i < nb_workers; i++) {
					fw.write("\t\t<route src=\"worker" + i + "\" dst=\"first-primary0\" symmetrical=\"YES\">\n");
					fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
					fw.write("\t\t</route>\n");
					
					nb_route++;
				}	
				
				// create the routes of the system (to primaries towards clients)
				for (int i = 0; i < nb_primaries; i++) {
					for (int j = 0; j < nb_clients; j++) {
						fw.write("\t\t<route src=\"client" + j + "\" dst=\"primary" + i + "\" symmetrical=\"YES\">\n");
						fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
						fw.write("\t\t</route>\n");
						
						nb_route++;
					}				
				}
				
				// create the routes of the system (to primaries towards workers)
				for (int i = 0; i < nb_primaries; i++) {
					for (int j = 0; j < nb_workers; j++) {
						fw.write("\t\t<route src=\"worker" + j + "\" dst=\"primary" + i + "\" symmetrical=\"YES\">\n");
						fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
						fw.write("\t\t</route>\n");
						
						nb_route++;
					}				
				}
				// create the routes of the system (to primaries towards primaries)
				for (int i = 0; i < nb_primaries; i++) {
					for (int j = i + 1; j < nb_primaries; j++) {
						if (i != j) {
							fw.write("\t\t<route src=\"primary" + j + "\" dst=\"primary" + i + "\" symmetrical=\"YES\">\n");
							fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
							fw.write("\t\t</route>\n");
							
							nb_route++;
						}
					}
				}
				
				
			}
			
			fw.write("\n");
			
			// end of the xml file
			fw.write("\t</AS>\n");
			fw.write("</platform>\n");	
			fw.flush();
		} catch (IOException e2) {
			e2.printStackTrace();
		}
	}
	
	
	public static void main (String [] args) {
		int nb_workers = 0;
		int nb_clients = 0;
		int nb_primaries = 1;
		
		if (args.length < 5) {
			System.out.println("you need to indicate the databse you want to use to retrieve informations");
			System.out.println("you need to indicate whether you want to simulate a CENTRALIZED or a DISTRIBUTED system");
			System.out.println("you need to indicate the maximum number of workers you want in the system.");
			System.out.println("you need to indicate the number of clients you want in the system.");
			System.out.println("you need to indicate the directory in which you want the xml files been written");
			return;			
		}
		
		if (args[1].equals("DISTRIBUTED")) {
			if (args.length != 6) {
				System.out.println("you need to indicate the number of primaries you want in the system");
				return;
			}
			nb_primaries = Integer.parseInt(args[4]);
		}
		
		nb_clients = Integer.parseInt(args[3]);
		
		try {
			Class.forName("com.mysql.jdbc.Driver").newInstance();

			// to replace with the database boinc (traces of seti@home)
			String url = "jdbc:mysql://localhost/" + args[0];
			String user = "marjo";
			String passwd = "marjo";
			
			Connection conn = DriverManager.getConnection(url, user, passwd);

			Statement stmt = conn.createStatement();
		
		    ResultSet rs = stmt.executeQuery("SELECT count(node_id) from node");

		    while(rs.next()){
		    	nb_workers = rs.getInt(1);
		    	System.out.println("number of workers wanted " + Integer.parseInt(args[2]));
		    	System.out.println("number of workers available " + nb_workers);
		    
		    	if (Integer.parseInt(args[2]) > nb_workers) {
		    		System.out.println("We only be able to have " + nb_workers + " workers in the system");
		    	}
		    	else {
		    		nb_workers = Integer.parseInt(args[2]);
		    	}
		    }
		    
		    if (args[1].equals("DISTRIBUTED")) {
			    create_xml_deployement(conn, nb_workers, nb_clients, nb_primaries, args[5]);
			    
			    create_xml_platform(conn, nb_workers, nb_clients, nb_primaries, args[5]);	
			}
			else {
				 create_xml_deployement(conn, nb_workers, nb_clients, nb_primaries, args[4]);
				    
				 create_xml_platform(conn, nb_workers, nb_clients, nb_primaries, args[4]);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}      
	}
}
	
