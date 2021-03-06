// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <map>
#include <set>

#include <tiny_dnn\config.h>
#include <tiny_dnn\tiny_dnn.h>
#include <tiny_dnn\util\image.h>
#include <tiny_dnn\layers\layer.h>
#include <tiny_dnn\layers\convolutional_layer.h>

#include <cpprest\http_listener.h>
#include <cpprest\json.h>
#include <base64.h>




using namespace std;

using namespace tiny_dnn;
using namespace tiny_dnn::layers;
using namespace tiny_dnn::activation;


using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace utility::conversions;


