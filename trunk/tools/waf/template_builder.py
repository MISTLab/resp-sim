#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
# encoding: utf-8
# John O'Meara, 2006

"Generate header"

import TaskGen, Task, Utils, Build
from Logs import error
import sys, os
import Options


def generate_header(task):
    # Set the path for importing additional tools
    curPath = os.path.abspath(os.path.join('..', os.path.dirname(sys.modules['template_builder'].__file__)))
    if not curPath in sys.path:
        sys.path.append(curPath)
    from pypp_utils import TemplateBuilder
    from waf_utils import rec_find

    # Generate templates
    tb = TemplateBuilder()
    templates = Utils.to_list(task.templates)
    for t in templates: tb.Template(t)
    header_content = tb.buildAutogenContents(task.target.upper()+'_EXP_HPP'
                                , [t.srcpath(task.env) for t in task.inputs])

    # Write extra header file
    temp_header_name = os.path.join(os.path.abspath('.'), task.outputs[0].bldpath(task.env))
    temp_header = open(temp_header_name ,'w')
    if header_content:
        temp_header.write(header_content)
    temp_header.close()

def setup(env):
    # create our action here
    Task.task_type_from_func(name='generate_header',func=generate_header, color='RED')


def detect(conf):
    return True

