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

#ifndef OPENLDEV_XML_PARSER_H
#define OPENLDEV_XML_PARSER_H

/*!
  \file openldev-xml-parser.h
  \brief OpenLDevXmlDocument
*/

#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

/*! \brief An XML document structure and the path to the xml file. */
struct OpenLDevXmlDocument
{
  /*! Path and name of the xml file. */
  gchar *xml_file;
  /*! Pointer to the libxml document structure. */
  xmlDocPtr xml_doc;
};

/*! Create a new xml document with parameter xml_file where to save it.
  \param xml_file Filename of the new xml file
 \return Pointer to an xml document object, NULL if failure occured
*/
OpenLDevXmlDocument* openldev_xml_document_new (gchar *xml_file);

/*! Open an existing xml document from path to xml_file.
  \param xml_file Name (with path) of the xml file
  \return Pointer to an xml document object, NULL if failure occured
*/
OpenLDevXmlDocument* openldev_xml_document_open (gchar *xml_file);

/*! Save the opened xml document.
  \param xml_document Address of pointer to the xml document
*/
void openldev_xml_document_save (OpenLDevXmlDocument** xml_document);

/*! Close a previously opened xml document. The xml document will be freed, and the xml document pointer is set to NULL.
  \param xml_document Address of pointer to the xml document
*/
void openldev_xml_document_close (OpenLDevXmlDocument** xml_document);

/*! Change the name under which the xml file will be saved.
  \param xml_document The xml document
  \param new_filename New file name (with path)
*/
void openldev_xml_document_change_filename (OpenLDevXmlDocument* xml_document,
                                            gchar *new_filename);

/*! Set a new root element. The old structure of the xml document will be lost.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the new root element
*/
void openldev_xml_document_set_root_element (OpenLDevXmlDocument* xml_document,
                                             gchar *element_name);

/*! Set an attribute in the root element. Value will be changed if attribute exists.
  \param xml_document Pointer to the xml document object
  \param attribute_name Name of the attribute
  \param value Value of the attribute
*/
void openldev_xml_document_set_root_attribute (OpenLDevXmlDocument* xml_document,
                                               gchar *attribute_name,
                                               gchar *value);

/*! Remove an attribute from the root element.
  \param xml_document Pointer to the xml document object
  \param attribute_name Name of the attribute
*/
void openldev_xml_document_remove_root_attribute (OpenLDevXmlDocument* xml_document,
                                                  gchar *attribute_name);

/*! Set an element with value as child to parent element. If element exists, it will be replaced. Function fails if either the parent element or the element could not be found.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the element
  \param value Value of the element
  \return True if element was set, otherwise false
*/
gboolean openldev_xml_document_set_element (OpenLDevXmlDocument* xml_document,
                                            gchar *element_name,
                                            gchar *value);

/*! Add new element as child to parent element. Use this function to add multiple elements with same name. Functions fails if parent element was not found.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the element
  \param value Value of the element
  \return TRUE if element was added, otherwise false
*/
gboolean openldev_xml_document_add_element (OpenLDevXmlDocument* xml_document,
                                            gchar *element_name,
                                            gchar *value);

/*! Set an attribute with value. If attribute is missing it will be created. Function fails if either the parent element or the element could not be found.
  \param xml_document Pointer to the xml document object
  \param parent_name Name of the parent element
  \param element_name Name of the element
  \param attribute_name Name of the attribute
  \param value New value of the attribute
  \return TRUE if value is set, otherwise false
*/
gboolean openldev_xml_document_set_attribute (OpenLDevXmlDocument* xml_document,
                                              gchar *parent_name,
                                              gchar *element_name,
                                              gchar *attribute_name,
                                              gchar *value);

/*! Return an elements value.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the element
  \return The value of the element, or an empty string if element was not found
*/
gchar* openldev_xml_document_element_value (OpenLDevXmlDocument* xml_document,
                                   			    gchar *element_name);

/*! Return a list of values of all elements named element_name.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the element
  \return A list of values, or an empty list if no element was found
*/
GSList*
openldev_xml_document_element_values (OpenLDevXmlDocument* xml_document,
                                      gchar *element_name);

/*! Return the value of an attribute from an element.
  \param xml_document Pointer to the xml document object
  \param parent_name Name of the parent element
  \param element_name Name of the element
  \param attribute_name Name of the attribute
  \return The value of the attribute, or an empty string if attribute was not found
*/
gchar* openldev_xml_document_attribute_value (OpenLDevXmlDocument* xml_document,
                                              gchar *parent_name,
                                              gchar *element_name,
                                              gchar *attribute_name);

/*! Return the value of an attribute from an element.
  \param xml_document Pointer to the xml document object
  \param attribute_name Name of the attribute
  \return The value of the attribute, or an empty string if attribute was not found
*/
gchar* openldev_xml_document_root_attribute (OpenLDevXmlDocument* xml_document,
                                             gchar *attribute_name);

/*! Remove a child element with given value from parent element. If the parameter value is empty all elements with given name will be removed.
  \param xml_document Pointer to the xml document object
  \param element_name Name of the element
  \param value The value of the element to remove, or empty to remove all elements
*/
void openldev_xml_document_remove_element (OpenLDevXmlDocument* xml_document,
                                   		 		 gchar *element_name,
                                   				 gchar *value);

/*! Remove all children from parent element.
  \param xml_document Pointer to the xml document object
  \param parent_element Name of the parent element
*/
void openldev_xml_document_remove_children (OpenLDevXmlDocument* xml_document,
                                            gchar *parent_element);

/*! Remove an attribute from an element.
  \param xml_document Pointer to the xml document object
  \param parent_name Name of the parent element
  \param element_name Name of the element
  \param attribute_name Name of the attribute
*/
void openldev_xml_document_remove_attribute (OpenLDevXmlDocument* xml_document,
                                             gchar *parent_name,
                                             gchar *element_name,
                                             gchar *attribute_name);

#endif //OPENLDEV_XML_PARSER_H
