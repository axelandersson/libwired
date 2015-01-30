/*
 *  Copyright (c) 2015 Axel Andersson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <wired/wired.h>
#include "test.h"

WI_TEST_EXPORT void                     wi_test_xml_parser_success(void);
WI_TEST_EXPORT void                     wi_test_xml_parser_failure(void);


void wi_test_xml_parser_success(void) {
#ifdef WI_XML
    wi_xml_node_t   *node, *child_node, *child_child_node, *child_child_child_node;
 
    node = wi_xml_parser_read_node_from_path(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-xml-parser-tests-1.xml")));
    
    WI_TEST_ASSERT_NOT_NULL(node, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(node), WI_STR("PurchaseOrder"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(node),
                                   wi_dictionary_with_data_and_keys(WI_STR("99503"), WI_STR("PurchaseOrderNumber"),
                                                                    WI_STR("1999-10-20"), WI_STR("OrderDate"),
                                                                    NULL),
                                   "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(node), 4U, "");
    
    child_node = wi_xml_node_child_at_index(node, 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_node), WI_STR("Address"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(child_node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_node),
                                   wi_dictionary_with_data_and_keys(WI_STR("Shipping"), WI_STR("Type"), NULL),
                                   "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_node), 6U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Name"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("Ellen Adams"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Street"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("123 Maple Street"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("City"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("Mill Valley"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("State"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("CA"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 4);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Zip"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("10999"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 5);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Country"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("USA"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_node = wi_xml_node_child_at_index(node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_node), WI_STR("Address"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(child_node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_node),
                                   wi_dictionary_with_data_and_keys(WI_STR("Billing"), WI_STR("Type"), NULL),
                                   "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_node), 6U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Name"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("Tai Yee"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Street"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("8 Oak Avenue"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("City"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("Old Town"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("State"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("PA"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 4);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Zip"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("95819"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 5);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Country"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_node), WI_STR("USA"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 0U, "");

    child_node = wi_xml_node_child_at_index(node, 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_node), WI_STR("DeliveryNotes"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_node), WI_STR("Please leave packages in shed by driveway."), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_node), 0U, "");
    
    child_node = wi_xml_node_child_at_index(node, 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_node), WI_STR("Items"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(child_node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_node), 2U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Item"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(child_child_node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node),
                                   wi_dictionary_with_data_and_keys(WI_STR("872-AA"), WI_STR("PartNumber"), NULL),
                                   "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 4U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 0);

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("ProductName"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("Lawnmower"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("Quantity"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("1"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("USPrice"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("148.95"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("Comment"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("Confirm this is electric"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_node = wi_xml_node_child_at_index(child_node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_node), WI_STR("Item"), "");
    WI_TEST_ASSERT_NULL(wi_xml_node_text(child_child_node), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_node),
                                   wi_dictionary_with_data_and_keys(WI_STR("926-AA"), WI_STR("PartNumber"), NULL),
                                   "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_node), 4U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("ProductName"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("Baby Monitor"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("Quantity"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("USPrice"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("39.98"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
    
    child_child_child_node = wi_xml_node_child_at_index(child_child_node, 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_element(child_child_child_node), WI_STR("ShipDate"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_text(child_child_child_node), WI_STR("1999-05-21"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_xml_node_attributes(child_child_child_node), wi_dictionary(), "");
    WI_TEST_ASSERT_EQUALS(wi_xml_node_number_of_children(child_child_child_node), 0U, "");
#endif
}



void wi_test_xml_parser_failure(void) {
#ifdef WI_XML
    wi_xml_node_t   *node;
    wi_data_t       *data;
    
    data = wi_data_with_contents_of_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-xml-parser-tests-2.xml")));
    node = wi_xml_parser_node_from_data(data);
    
    WI_TEST_ASSERT_NULL(node, "");
#endif
}
