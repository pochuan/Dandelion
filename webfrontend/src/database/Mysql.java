package database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class Mysql {
	static final String account = "ccs108ghall1";
	static final String password = "oovaejee";
	static final String server = "mysql-user.stanford.edu";
	static final String datName = "c_cs108_ghall1";
	//class database name
	Connection con;
	
	/**
	 * Creates a new database connection using the information in the class variable strings
	 */
	public Mysql(){
		//open connection
		try {
			Class.forName("com.mysql.jdbc.Driver");
			con = DriverManager.getConnection("jdbc:mysql://" + server, account, password);
		} catch (SQLException e) {
			e.printStackTrace();
		}
		catch (ClassNotFoundException e){
			e.printStackTrace();
		}
		Statement stmt;
		//set the database name we are using
		try {
			stmt = con.createStatement();
			stmt.executeQuery("USE " + datName);
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	/**
	 * Querys a database given a string and returns the result set from that query
	 * @param query A query string of what to send the database
	 * @return Results Set of the query
	 */
	public ResultSet queryDatabase(String query){
		//Set the table we want to use
		Statement stmt;
		ResultSet rs = null;
		//query database
		try {
			stmt = con.createStatement();
			rs = stmt.executeQuery(query);
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return rs;
	}
	/**
	 * The method used to send all commands which modify 
	 * the database including, DELETE, INSERT, and UPDATE
	 * @param update String to send to the SQL Database
	 */
	public void updateDatabase(String update){
		Statement stmt;
		try{
			stmt = con.createStatement();
			stmt.executeUpdate(update);
		} catch (SQLException e){
			e.printStackTrace();
		}
	}
}
