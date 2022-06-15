import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;


import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;
import java.io.FileWriter;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;


public class ReadXmlDomParser {

  private static final String FILENAME = "test.net.xml";

  public static String removeCharAt(String s, int pos) {
    return s.substring(0, pos) + s.substring(pos + 1);
  }

  public static void findPathWithId (NodeList listconnection, NodeList listlane, String id2) {
    
    try {
      boolean visit [] = new boolean[10000];
      FileWriter myWriter = new FileWriter("output.txt");
      Queue<String> q = new LinkedList<>();
      q.add(id2);
      int count = 0;

      while (q.size() != 0){
  
        if(((count ++) % 100) == 0) {
        	System.out.println(count);
        }
        String printf = q.peek();
        boolean b = false ;
        String shape = "";
        
        for (int i= 0; i< listlane.getLength(); i++){
          Element ele = (Element) listlane.item(i);
          String eleId = ele.getAttribute("id");

          if (eleId.equals(q.peek())){
        	  shape = " " + ele.getAttribute("shape");
            printf = printf + " " + ele.getAttribute("length"); 
            break;
          }
        }

        for (int i = 0; i < listconnection.getLength() ; i++) {   

          Element ele = (Element) listconnection.item(i);

          String elefrom = ele.getAttribute("from") + "_" + ele.getAttribute("fromLane");

          String eleto = ele.getAttribute("to") + "_" + ele.getAttribute("toLane");

          String elevia = ele.getAttribute("via");

          if (elefrom.equals(q.peek()) && visit[i] == false){
            b = true;
            if (elevia != ""){
              q.add(elevia);
              printf = printf + " " + elevia;
            } else {
              q.add(eleto);
              printf = printf + " " + eleto;
            }
            visit[i] = true;
          }
        }
        if (b == true) myWriter.write(/*count + " " +*/ printf + shape + "\n");
        q.remove();
      }
      //myWriter.write("Ghi vao file output.txt thanh cong");
      myWriter.close();
    }
    catch (IOException e){
      e.printStackTrace();
    }
  }

  public static void findEdgeWithId (NodeList listlane, String id1){
    boolean b = new Boolean(true);
        b = true;
        for (int i = 0 ; i < listlane.getLength(); i++ ){
          
          
          Element ele = (Element) listlane.item(i);
          Element eleParent = (Element) ele.getParentNode();
          String eleId = ele.getAttribute("id");

          if (eleId.equals(id1.trim())){
            b = false;
            System.out.println("id cua edge la " + eleParent.getAttribute("id"));
            System.out.println("index cua lane la " + ele.getAttribute("index"));
            System.out.println("do dai cua lane la " + ele.getAttribute("length"));
  
          }
 
        }

        if (b == true) {
          System.out.println("khong tim duoc edge co id tuong ung");
        }

  }

  public static void listCrossing(NodeList listlane) throws IOException {
    FileWriter myWriter = new FileWriter("crossing.txt");
    String printf;

    for (int i = 0 ; i < listlane.getLength(); i++) {
      Element ele = (Element) listlane.item(i);
      Element eleParent = (Element) ele.getParentNode();
      String eleParentFunction = eleParent.getAttribute("function");

      if (eleParentFunction.compareTo("crossing") == 0) {
        printf = ele.getAttribute("id");
        printf = printf + " " + ele.getAttribute("length");
        printf = printf + " " + eleParent.getAttribute("crossingEdges");
        
        Point2D MN[] = Point2D.strToListPoint(ele.getAttribute("shape"));
        Point2D ABCD[] = Point2D.calVertex(MN[0], MN[1]);
        printf = printf + Point2D.listPointToStr(ABCD);
        myWriter.write(printf + "\n");
      }
    }

    myWriter.close();
  }

  public static void listCrossingUpdate(NodeList listlane) throws IOException {
    Map<String, Integer> idAndCount = new HashMap<>();
    FileWriter myWriter = new FileWriter("crossing.txt");
    String printf = new String();

    // build hash table with count
    for (int i = 0 ; i < listlane.getLength(); i++) {
      Element ele = (Element) listlane.item(i);
      Element eleParent = (Element) ele.getParentNode();
      String eleParentFunction = eleParent.getAttribute("function");

      if (eleParentFunction.compareTo("crossing") == 0) {
        String edgeId = eleParent.getAttribute("id");
        String junctionId = edgeId.split("_")[0];
        Integer count = idAndCount.get(junctionId);
        if (count == null) {
            idAndCount.put(junctionId, 1);
        } else {
            idAndCount.put(junctionId, ++count);
        }
      }
    }

    for (int i = 0 ; i < listlane.getLength(); i++) {
      Element ele = (Element) listlane.item(i);
      Element eleParent = (Element) ele.getParentNode();
      String eleParentFunction = eleParent.getAttribute("function");

      if (eleParentFunction.compareTo("crossing") == 0) {
        String laneId = ele.getAttribute("id");
        String junctionId = laneId.split("_")[0];
        if (idAndCount.get(junctionId) > 1) {
          printf = "I " + laneId;
        } else {
          printf = "L " + laneId;
        }
        printf = printf + " " + ele.getAttribute("length");
        printf = printf + " " + eleParent.getAttribute("crossingEdges");
        
        Point2D MN[] = Point2D.strToListPoint(ele.getAttribute("shape"));
        Point2D ABCD[] = Point2D.calVertex(MN[0], MN[1]);
        printf = printf + Point2D.listPointToStr(ABCD);
        myWriter.write(printf + "\n");
      }
    }

    myWriter.close();
  }

  public static void main(String[] args) {
    
    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    
    try {
      
        Scanner sc = new Scanner(System.in);
        // Read XMl file
        
        DocumentBuilder db = dbf.newDocumentBuilder();
        
        Document Rdoc = db.parse(new File(FILENAME));
        
        Rdoc.getDocumentElement().normalize();
        
        System.out.println("Root Element :" + Rdoc.getDocumentElement().getNodeName());
        System.out.println("------");
        
        // get <lane>
        // get <connection>
        NodeList listlane = Rdoc.getElementsByTagName("lane");
        NodeList listconnection = Rdoc.getElementsByTagName("connection");

        // System.out.println("Liet ke danh sach crossing ... done");
        // listCrossing(listlane);

        System.out.println("Liet ke danh sach crossing updated ... done");
        listCrossingUpdate(listlane);
        
        // findEdgeWithId

        // System.out.println("Nhap vao id de tim edge tuong ung");
        // String id1 = sc.nextLine();       
        // findEdgeWithId(listlane, id1);
       

        // findPathWithId
        // System.out.println("Nhap vao 1 id de tim duong di");
        // String id2 = sc.nextLine();
        // System.out.println("");

        // findPathWithId (listconnection, listlane, id2);

        sc.close();

      } catch (ParserConfigurationException | SAXException | IOException e) {
          e.printStackTrace();
      } catch(Exception ex)
      {
        ex.printStackTrace();
      }

  }

}

class Point2D {
  double x;
  double y;

  public Point2D(){}

  public Point2D(String str) {
    String[] coord = str.split(",");

    this.x = Double.parseDouble(coord[0]);
    this.y = Double.parseDouble(coord[1]);
  }

  public String pointToStr() {
    String printf = String.format("%.2f", this.x);
    printf = printf + ",";
    printf = String.format("%.2f", this.y);
    return printf;
  }

  public static Point2D[] strToListPoint(String str) {
    String points[] = str.split(" ");
    Point2D list[] = new Point2D[points.length];

    for (int i = 0; i < points.length; i++) {
      list[i] = new Point2D(points[i]);
    }

    return list;
  }

  public static String listPointToStr(Point2D list[]) {
    String printf = new String();

    for (int i = 0; i < list.length; i++) {
      printf = printf + " " + String.format("%.2f", list[i].x);
      printf = printf + "," + String.format("%.2f", list[i].y);
    }
    
    return printf;
  }

  public static Point2D[] calVertex(Point2D M, Point2D N) {
    Point2D A = new Point2D();
    Point2D B = new Point2D();
    Point2D C = new Point2D();
    Point2D D = new Point2D();

    if (M.x == N.x) {
      A.x = D.x = M.x + 2;
      B.x = C.x = M.x - 2;
      A.y = B.y = M.y;
      C.y = D.y = N.y;
    } else {
      A.x = B.x = M.x;
      C.x = D.x = N.x;
      A.y = D.y = M.y + 2;
      B.y = C.y = M.y - 2;
    }

    Point2D points[] = {A, B, C, D};
    return points;
  }
}