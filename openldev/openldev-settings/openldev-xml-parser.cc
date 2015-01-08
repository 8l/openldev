/***************************************************************************
 *	 Copyright (C) 2006 by Uwe Grawert                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.              *
 ***************************************************************************/

#include "openldev-xml-parser.h"
#include <cstring>

// Create a new xml document with parameter xml_file where to save it
OpenLDevXmlDocument* 
openldev_xml_document_new (gchar *xml_file)
{
  OpenLDevXmlDocument *newXmlDocument = g_slice_new (OpenLDevXmlDocument);

  newXmlDocument->xml_file = g_strdup (xml_file);
  newXmlDocument->xml_doc = xmlNewDoc ((xmlChar*) "1.0");

  return newXmlDocument;
}

// Open an xml document from xml_file
OpenLDevXmlDocument* 
openldev_xml_document_open (gchar *xml_file)
{
  OpenLDevXmlDocument *open_xml_doc;

  xmlInitParser();
  open_xml_doc = g_slice_new (OpenLDevXmlDocument);
  open_xml_doc->xml_doc = xmlReadFile (xml_file, NULL, 0);

  if (open_xml_doc->xml_doc != NULL)
    open_xml_doc->xml_file = g_strdup (xml_file);
  // Error reading the project file
  else
  { 
    delete open_xml_doc; 
    open_xml_doc = NULL; 
  }

  xmlCleanupParser();
  return open_xml_doc;
}

// Save an xml document
void 
openldev_xml_document_save (OpenLDevXmlDocument** xml_document)
{
  OpenLDevXmlDocument *xml_doc_ptr = *xml_document;

  xmlCleanupParser();
  if (xml_doc_ptr != NULL)
    xmlSaveFormatFileEnc (xml_doc_ptr->xml_file, xml_doc_ptr->xml_doc, "UTF-8", 1);
}

// Close an xml document
void 
openldev_xml_document_close (OpenLDevXmlDocument** xml_document)
{
  OpenLDevXmlDocument *xml_doc_ptr = *xml_document;

  if (xml_doc_ptr != NULL)
  {
		xmlKeepBlanksDefault (0);
    xmlSaveFormatFileEnc (xml_doc_ptr->xml_file, xml_doc_ptr->xml_doc, "UTF-8", 1);
    xmlFreeDoc (xml_doc_ptr->xml_doc);
    g_slice_free (OpenLDevXmlDocument, xml_doc_ptr); 
    *xml_document = NULL;
  }
}

// Change the name under which the xml file will be saved
void 
openldev_xml_document_change_filename (OpenLDevXmlDocument* xml_document,
                                       gchar *new_filename)
{
  if (xml_document != NULL)
    xml_document->xml_file = g_strdup (new_filename);
}

// Set a new root element. The old structure of the xml document will be lost
void 
openldev_xml_document_set_root_element (OpenLDevXmlDocument* xml_document,
                                        gchar *element_name)
{
  xmlNodePtr new_node;

  if (xml_document != NULL)
  {
    new_node = xmlNewNode (NULL, (xmlChar*) element_name);
    xmlDocSetRootElement (xml_document->xml_doc, new_node);
    xmlFree (new_node);
  }
}

// Set an attribute in the root element
void 
openldev_xml_document_set_root_attribute (OpenLDevXmlDocument* xml_document,
                                          gchar *attribute_name,
                                          gchar *value)
{
  xmlNodePtr root_node;

  if (xml_document != NULL)
  {
    root_node = xmlDocGetRootElement (xml_document->xml_doc);
    xmlSetProp (root_node, (xmlChar*) attribute_name, (xmlChar*) value);
  }
}

// Remove an attribute from the root element
void
openldev_xml_document_remove_root_attribute (OpenLDevXmlDocument* xml_document,
                                             gchar *attribute_name)
{
  xmlNodePtr root_node;
  xmlAttrPtr del_attr;

  if (xml_document != NULL)
  {
    root_node = xmlDocGetRootElement (xml_document->xml_doc);
    del_attr  = xmlHasProp (root_node, (xmlChar*) attribute_name);
    xmlRemoveProp (del_attr);
  }
}

// Search for the element with name starting at root_node
xmlNodePtr 
find_xmlNode_by_name (xmlNodePtr root_node,
                      const xmlChar* element_name)
{
  xmlNodePtr key_node = NULL;

  root_node = root_node->children;

  while (root_node != NULL)
  {
    if ((root_node->type == XML_ELEMENT_NODE) && !xmlStrcmp (root_node->name, element_name))
      return root_node;
    else
    {
      if (root_node->children)
        key_node = find_xmlNode_by_name (root_node->children, element_name);
      if (key_node != NULL)
        return key_node;
    }

    root_node = root_node->next;
  }

  return NULL;
}

// Set an element with value. Value will be changed if element exists
gboolean 
openldev_xml_document_set_element (OpenLDevXmlDocument* xml_document,
                                   gchar *element_name,
                                   gchar *value)
{
  xmlNodePtr root_node, key_node, new_node;

  if (xml_document == NULL)
    return FALSE;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  key_node = find_xmlNode_by_name (root_node, (xmlChar*) element_name);
  
  if (key_node != NULL)
    xmlNodeSetContent (key_node, (xmlChar*) value);
  else
  {
    new_node = xmlNewNode (NULL, (xmlChar*) element_name);
    xmlNodeSetContent (new_node, (xmlChar*) value);
    xmlAddChild (root_node, new_node);
  }

  return TRUE;
}

// Add new element as child to parent element
gboolean 
openldev_xml_document_add_element (OpenLDevXmlDocument* xml_document,
                                   gchar *element_name,
                                   gchar *value)
{
  xmlNodePtr root_node, new_node;

  if (xml_document == NULL)
    return FALSE;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  new_node = xmlNewNode (NULL, (xmlChar*) element_name);
  xmlNodeSetContent (new_node, (xmlChar*) value);
  xmlAddChild (root_node, new_node);

  return TRUE;
}

// Set an attribute with value. If attribute is missing it will be created
gboolean 
openldev_xml_document_set_attribute (OpenLDevXmlDocument* xml_document,
                                     gchar *parent_name,
                                     gchar *element_name,
                                     gchar *attribute_name,
                                     gchar *value)
{
  xmlNodePtr root_node, parent_node, key_node;

  if(xml_document == NULL)
    return FALSE;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  parent_node = find_xmlNode_by_name (root_node, (xmlChar*) parent_name);
  key_node = find_xmlNode_by_name (parent_node, (xmlChar*) element_name);

  if (key_node == NULL)
    return FALSE;

  xmlSetProp(key_node, (xmlChar*) attribute_name, (xmlChar*) value);
  return TRUE;
}

// Return the value for element named element_name
gchar*
openldev_xml_document_element_value (OpenLDevXmlDocument* xml_document,
                                     gchar *element_name)
{
  gchar *ret_string;
  xmlNodePtr root_node, key_node;
  xmlChar *result;

  if (xml_document == NULL)
    return ret_string;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  key_node = find_xmlNode_by_name (root_node, (xmlChar*) element_name);
  result = xmlNodeGetContent (key_node);

  if (result != NULL)
  {
    ret_string = g_strdup ((gchar*) result);
    xmlFree (result);
  }

  return ret_string;
}

// Return a list of property values of all elements named element_name
GSList*
openldev_xml_document_element_values (OpenLDevXmlDocument* xml_document,
                                      gchar *element_name)
{
  GSList *ret_list = NULL;
  xmlChar *value, *ckey;
  xmlNodePtr root_node, key_node, itr;

  if (xml_document == NULL)
    return ret_list;

  ckey = (xmlChar*) element_name;
  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  key_node = find_xmlNode_by_name (root_node, ckey);

  for (itr = key_node; itr != NULL; itr = itr->next)
  {
    value = NULL;
    
    if ((itr->type == XML_ELEMENT_NODE) && !xmlStrcmp (itr->name, ckey))
    {
      value = xmlNodeGetContent(itr);
      if (value != NULL)
      {
        ret_list = g_slist_append (ret_list, (gpointer) g_strdup ((gchar*) value));
        xmlFree (value);
      }
    }
  }

  return ret_list;
}

// Return the value of an attribute from element
gchar*
openldev_xml_document_root_attribute (OpenLDevXmlDocument* xml_document,
                                      gchar *attribute_name)
{
  gchar *ret_string;
  xmlChar *cattr_val;
  xmlNodePtr root_node;

  if (xml_document == NULL)
    return ret_string;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  cattr_val = xmlGetProp (root_node, (xmlChar*) attribute_name);

  if (cattr_val != NULL)
  {
    ret_string = g_strdup ((gchar*) cattr_val);
    xmlFree (cattr_val);
  }

  return ret_string;
}

// Return the value of an attribute from element
gchar* 
openldev_xml_document_attribute_value (OpenLDevXmlDocument* xml_document,
                                       gchar *parent_name,
                                       gchar *element_name,
                                       gchar *attribute_name)
{
  gchar *ret_string;
  xmlChar *cattr_val;
  xmlNodePtr root_node, parent_node, key_node;

  if (xml_document == NULL)
    return ret_string;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  parent_node = find_xmlNode_by_name (root_node, (xmlChar*) parent_name);
  key_node = find_xmlNode_by_name (parent_node, (xmlChar*) element_name);
  cattr_val = xmlGetProp (key_node, (xmlChar*) attribute_name);

  if (cattr_val != NULL)
  {
    ret_string = g_strdup ((gchar*) cattr_val);
    xmlFree (cattr_val);
  }

  return ret_string;
}

// Remove a child element with given value from parent element
void 
openldev_xml_document_remove_element (OpenLDevXmlDocument* xml_document,
                                   		gchar *element_name,
                                   		gchar *value)
{
  xmlChar *ckey, *cval;
  xmlNodePtr root_node, remove_node, itr;
  gboolean must_break;

  if(xml_document == NULL)
    return;

  ckey = (xmlChar*) element_name;
  cval = (xmlChar*) value;
  root_node = xmlDocGetRootElement (xml_document->xml_doc);

  if (strlen (value) == 0)
  {
    for (itr = root_node->children; itr != NULL; itr = itr->next)
    {
      if ((itr->type == XML_ELEMENT_NODE) && !xmlStrcmp (itr->name, ckey))
      {
        remove_node = itr;
        itr = itr->next;
        xmlUnlinkNode (remove_node);
        xmlFreeNode (remove_node);
      }
    }
  }
  else
  {
    must_break = FALSE;
    
    for (itr = root_node->children; itr != NULL; itr = itr->next)
    {
      if (itr->next == NULL)
        must_break = TRUE;
    
      if((itr->type == XML_ELEMENT_NODE) && !xmlStrcmp (itr->name, ckey))
        if (!xmlStrcmp (xmlNodeGetContent(itr), cval))
        {
          remove_node = itr;
          itr = itr->next;
          xmlUnlinkNode (remove_node);
          xmlFreeNode (remove_node);
        }
      
      if (must_break)
        break;
    }
  }
}

// Remove all children from parent element
void 
openldev_xml_document_remove_children (OpenLDevXmlDocument* xml_document,
                                       gchar *parent_element)
{
  xmlNodePtr root_node, parent_node, remove_node, itr;

  if (xml_document == NULL)
    return;

  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  parent_node = find_xmlNode_by_name (root_node, (xmlChar*) parent_element);

  if (parent_node == NULL)
    return;

  for (itr = parent_node->children; itr != NULL; itr = itr->next)
  {
      remove_node = itr;
      itr = itr->next;
      xmlUnlinkNode (remove_node);
      xmlFreeNode (remove_node);
  }
}

// Remove an attribute from an element
void 
openldev_xml_document_remove_attribute (OpenLDevXmlDocument* xml_document,
                                        gchar *parent_name,
                                        gchar *element_name,
                                        gchar *attribute_name)
{
  xmlAttrPtr del_attr;
  xmlNodePtr root_node, parent_node, key_node;
  
  
  root_node = xmlDocGetRootElement (xml_document->xml_doc);
  parent_node = find_xmlNode_by_name (root_node, (xmlChar*) parent_name);
  key_node = find_xmlNode_by_name (parent_node, (xmlChar*) element_name);
  
  del_attr = xmlHasProp (key_node, (xmlChar*) attribute_name);
  xmlRemoveProp (del_attr);
}
