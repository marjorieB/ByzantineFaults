import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Random;


public class Create_xml_files_centralised {
	
	
	public static void create_xml_deployement_centralised(Connection conn, int nb_workers, int nb_clients) {
		FileWriter fw = null;
		
		try {
			// create the header
			fw = new FileWriter ("/home/marjo/stage/logiciel/Simgrid_et_co/projet/platforms/deployement_centralized_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			fw.write("<?xml version='1.0'?>\n");
			fw.write("<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n");
			fw.write("<platform version=\"3\">\n\n");
		
			// indicate parameters for primary
			fw.write("\t<process host=\"primary\" function=\"primary\">\n");
			fw.write("\t\t<argument value=\"1\"/> <!-- id of the primary -->\n");
			fw.write("\t\t<argument value=\"" + nb_clients + "\"/> <!-- number of clients -->\n");
			fw.write("\t</process>\n");
			
			fw.write("\n");
			fw.flush();
			
			// indicate parameters for clients
			for (int i = 0; i < nb_clients; i++) {
				fw.write("\t<process host=\"client" + i + "\" function=\"client\">\n");
				fw.write("\t\t<argument value=\"" + i + "\"/> <!-- id of the client -->\n");
				fw.write("\t\t<argument value=\"primary-1\"/> <!-- name of the primary -->\n");
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
				rs = stmt.executeQuery("select node_id from node limit " + nb_workers);
				while(rs.next()){
					fw.write("\t<process host=\"worker" + i + "\" function=\"worker\">\n");
					fw.write("\t\t<argument value=\"" + rs.getInt(1) + "\"/> <!-- id of the worker -->\n");
					fw.write("\t\t<argument value=\"primary-1\"/> <!-- name of the primary -->\n");
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
	
	public static void create_xml_platform_centralised(Connection conn, int nb_workers, int nb_clients) {
		FileWriter fw = null;
		int nb_route = 0;
		
		try {
			// create the new file
			fw = new FileWriter ("/home/marjo/stage/logiciel/Simgrid_et_co/projet/platforms/platform_centralized_" + nb_workers + "workers_" + nb_clients + "clients.xml");
			// write the header
			fw.write("<?xml version='1.0'?>\n");
			fw.write("<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n");
			fw.write("<platform version=\"3\">\n\n");
			
			fw.write("\t<config id=\"general\">\n");
			fw.write("\t\t <prop id=\"network/model\" value=\"LV08\"/>\n");
			fw.write("\t</config>\n");
			
			fw.write("\t<AS id=\"AS0\" routing=\"Full\">\n\n");
			
			// create the hosts of the system
			fw.write("\t\t<host id=\"primary\" power=\"2000000000\"/>\n\n");
		
			fw.flush();
			
			for (int i = 0; i < nb_clients; i++) {
				fw.write("\t\t<host id=\"client" + i +"\" power=\"2000000000\"/>\n");
			}
			fw.write("\n");
			for (int i = 0; i < nb_workers; i++) {
				fw.write("\t\t<host id=\"worker" + i +"\" power=\"2000000000\"/>\n");
			}
			fw.write("\n");
			
			fw.flush();
			
			// create the links of the system
			for (int i = 0; i < nb_clients + nb_workers; i++) {
				fw.write("\t\t<link id=\"link" + i +"\" bandwidth=\"7.24MBps\" latency=\"0.004\"/>\n");
			}
			fw.write("\n");
			
			// create the route of the system (to client towards primary)
			for (int i = 0; i < nb_clients; i++) {
				fw.write("\t\t<route src=\"client" + i + "\" dst=\"primary\" symmetrical=\"YES\">\n");
				fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
				fw.write("\t\t</route>\n");
				
				nb_route++;
			}
			
			fw.write("\n");
			
			// create the route of the system (to primary to workers)
			for (int i = 0; i < nb_workers; i++) {
				fw.write("\t\t<route src=\"worker" + i + "\" dst=\"primary\" symmetrical=\"YES\">\n");
				fw.write("\t\t\t<link_ctn id=\"link" + nb_route + "\"/>\n");
				fw.write("\t\t</route>\n");
				
				nb_route++;
			}
			
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
		
		if (args.length != 2) {
			System.out.println("you need to indicate the maximum number of workers you want in the system.");
			System.out.println("you need to indicate the number of clients you want in the system.");
			return;			
		}
		nb_clients = Integer.parseInt(args[1]);
		
		try {
			Class.forName("com.mysql.jdbc.Driver").newInstance();

			// to replace with the database boinc (traces of seti@home)
			String url = "jdbc:mysql://localhost/test";
			String user = "marjo";
			String passwd = "marjo";
			
			Connection conn = DriverManager.getConnection(url, user, passwd);

			Statement stmt = conn.createStatement();
		
		    ResultSet rs = stmt.executeQuery("SELECT count(node_id) from node");

		    while(rs.next()){
		    	nb_workers = rs.getInt(1);
		    	System.out.println("number of workers wanted " + Integer.parseInt(args[0]));
		    	System.out.println("number of workers available " + nb_workers);
		    
		    	if (Integer.parseInt(args[0]) > nb_workers) {
		    		System.out.println("We only be able to have " + nb_workers + " in the system");
		    	}
		    	else {
		    		nb_workers = Integer.parseInt(args[0]);
		    	}
		    }
		    
		    create_xml_deployement_centralised(conn, nb_workers, nb_clients);
		    
		    create_xml_platform_centralised(conn, nb_workers, nb_clients);		    
		 
			
		} catch (Exception e) {
			e.printStackTrace();
		}      
	}
}
	
