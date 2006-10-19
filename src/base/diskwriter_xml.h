/*
 $Id: diskwriter_xml.h,v 1.2 2006/10/19 05:58:00 ksterker Exp $
 
 Copyright (C) 2006 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file base/diskwriter_xml.h 
 * @author Kai Sterker <kaisterker@linuxgames.com>
 *
 * @brief Read/write xml-formatted data files.
 */

#ifndef BASE_DISKWRITER_XML
#define BASE_DISKWRITER_XML

#include <libxml/tree.h>
#include "base/diskwriter_base.h"

namespace base {
    
    /**
     * This class provides a file writing/loading interface to the data flattener.
     * It allows to write a flat object to a gz compressed file, including a checksum 
     * for detecting data corruption when loading again later.
     */
    class disk_writer_xml : public disk_writer_base
    {
    public:
        /**
         * Parser states.
         */
        enum { UNDEF, DATA, LIST, PARAM };
        
        /**
         *
         */
        bool put_state (const std::string & name, base::flat & data) const;
    
        /**
         *
         */
        bool get_state (const std::string & name, base::flat & data) const;
        
#ifndef SWIG
        /// Table storing hex characters
        static char *Bin2Hex;

        /// make this class available to python::pass_instance
        GET_TYPE_NAME(base::disk_writer_xml)
    private:
        /**
         * Add the given record as child to the given xml element.
         * @param record record to serialize to XML structure.
         * @param parent node to which record should be added.
         */
        void record_to_xml (base::flat &record, xmlNodePtr parent) const;
        
        /**
         * Convert the given value to an xml character string.
         * @param type data type of given value.
         * @param value data to convert.
         * @param size length of data.
         * @return string representation of given value.
         */ 
        xmlChar *value_to_xmlChar (const flat::data_type & type, void *value, const u_int32 & size) const;
#endif
    };
}

#endif // BASE_DISKWRITER_XML
