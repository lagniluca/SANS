import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import java.io.File;
import java.util.ArrayList;

/*
 *  Parser for a condensed XML result.
 */
public class Condensed_Parser extends Parser
{
    //Constructors
    public Condensed_Parser(String filename)
    {
        this.filename = filename;
    }

    /*
     *  Parsing procedure
     */
    public void parse() throws Exception
    {
        SAXParserFactory spf = SAXParserFactory.newDefaultInstance();
        SAXParser saxParser = spf.newSAXParser();
        XMLReader parser = saxParser.getXMLReader();

        saxParser.parse(new File(filename), new SansHandler());
    }

    /*
     *  Inner class for SAX events' management
     */
    class SansHandler extends DefaultHandler
    {
        @Override
        public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException
        {
            if(qName.equalsIgnoreCase("module"))
            {
                tmp_mod = new Module();
            }
            if( qName.equalsIgnoreCase("signal"))
            {
                tmp_csig = new Condensed_Signal();
            }
        }

        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException
        {
            if(qName.equalsIgnoreCase("module"))
            {
                modules.add(tmp_mod);
            }
            if(qName.equalsIgnoreCase("signal"))
            {
                tmp_mod.addSignal(tmp_csig);
            }
            if(qName.equalsIgnoreCase("signalName"))
            {
                tmp_csig.setName(tmp_str);
            }
            if(qName.equalsIgnoreCase("name"))
            {
                tmp_mod.setModuleName(tmp_str);
            }
            if(qName.equalsIgnoreCase("slice"))
            {
                tmp_csig.setSlice(tmp_str);
            }
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException
        {
            tmp_str = new String(ch, start, length);
        }
    }
}