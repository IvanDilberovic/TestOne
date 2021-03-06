#include "stdafx.h"

#define TRACE(msg) wcout << msg
#define TRACE_ACTION(a, k, v) wcout << a << L" (" << k << L", " << v << L")\n"

map<utility::string_t, utility::string_t> dictionary;
vector <web::json::value> results;


template <typename Activation>
double rescale(double x) {

	Activation a(1);
	return 100.0 * (x - a.scale().first) / (a.scale().second - a.scale().first);
}

void convert_image(const std::string &imagefilename, double minv, double maxv, int w, int h, tiny_dnn::vec_t &data)
{
	tiny_dnn::image<> img(imagefilename, tiny_dnn::image_type::grayscale);

	tiny_dnn::image<> resized = resize_image(img, w, h);


	std::transform(resized.begin(), resized.end(), std::back_inserter(data), [=](uint8_t c)
	{
		return (255 - c) * (maxv - minv) / 255.0 + minv;
	});
}

void recognize(const std::string &dictionary, const std::string &src_filename) {

	tiny_dnn::network<tiny_dnn::sequential> nn;
	
	nn.load(dictionary, tiny_dnn::content_type::weights_and_model, tiny_dnn::file_format::json);

	// convert imagefile to vec_t
	tiny_dnn::vec_t data;

	convert_image(src_filename, -1.0, 1.0, 32, 32, data);

	// RECOGNIZE...............
	auto res = nn.predict(data); //vrati sve

	auto label = nn.predict_label(data); //vrati label
	auto max = nn.predict_max_value(data); //vrati max vrijednost	

	std::vector<std::pair<double, int>> scores;	

	//sort & print top - 3 
	for (int i = 0; i < 10; i++)
	{
		scores.emplace_back(rescale<tiny_dnn::activation::softmax>(res[i]), i); //tanh -> softmax promjena
	}

	sort(scores.begin(), scores.end(), std::greater<std::pair<double, int>>());

	for (int i = 0; i < 3; i++)
	{
		std::cout << scores[i].second << "," << scores[i].first << std::endl;

		web::json::value obj;

		obj[U("rezultat")] = web::json::value(scores[i].second);
		obj[U("postotak")] = web::json::value(scores[i].first);

		results.push_back(obj);

		obj.null;

	}
	
	// sejva svaki sliku layera // pogledaj da se povećaju slike
	for (size_t i = 0; i < nn.depth(); i++) {

		auto out_img = nn[i]->output_to_image();

		auto filename = "layer_" + std::to_string(i) + ".png";
				
		out_img.save(filename);
	}

	// save weights
	{
		auto weight = nn.at<tiny_dnn::convolutional_layer>(0).weight_to_image();
				
		auto filename = "weights.png";
				
		weight.save(filename);
	}


}

void handle_options(http_request request)
{

	http_response response(status_codes::OK);
	response.headers().add(U("Allow"), U("GET, POST, OPTIONS"));
	response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
	response.headers().add(U("Access-Control-Allow-Methods"), U("GET, POST, OPTIONS"));
	response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
	request.reply(response);
}

void handle_get(http_request request)
{
	TRACE(L"\nhandle GET\n");

	json::value obj;

	for (auto const & p : dictionary)
	{
		obj[p.first] = json::value::string(p.second);
	}

	utility::stringstream_t stream;

	obj.serialize(stream);

	std::wcout << stream.str();

	http_response response(status_codes::OK);

	response.headers().add(U("Access-Control-Allow-Origin"), U("*"));

	response.set_body(obj);

	request.reply(response);
}

void handle_post(http_request request)
{
	TRACE(L"\nhandle POST\n");

	/*auto test = request.extract_json(true);

	auto json = test.get();

	utility::string_t slika1 = json.as_string();

	string novi = utility::conversions::to_utf8string(slika1);

	string x = "";

	for (char& c : novi) {

		if (c != ',')
		{
			x = x + c + " ";
		}
	}

	std::stringstream iss(x);

	int number;
	std::vector<int> myNumbers;
	while (iss >> number)
	{
		myNumbers.push_back(number);
	}

	tiny_dnn::vec_t data(myNumbers.begin(), myNumbers.end());*/

	//string s = base64::decode(utility::conversions::to_utf8string(slika1));


	std::string model = "D:\\Projects\\Test\\TestOne\\TestOne\\Lenet-model-second";
	//string model = "D:\\Projects\\Test\\TestOne\\TestOne\\LeNet-weights";
	std::string slika = "D:\\Projects\\Test\\TestOne\\TestOneWeb\\App_Data\\NacrtanaSlika.png";

	try
	{
		recognize(model, slika);
	}
	catch (const std::exception& ex)
	{
		cout << ex.what();
	}


	std::cout << std::endl;

	http_response response(status_codes::OK);

	response.headers().add(U("Access-Control-Allow-Origin"), U("*"));

	web::json::value myJSON;

	//myJSON[U("rezultati")] = web::json::value::array(results);

	response.set_body(web::json::value::array(results));

	request.reply(response);

	results.clear();

}


int main(int argc, char **argv) {

	http_listener listener(L"http://localhost:8080/restdemo");

	listener.support(methods::OPTIONS, handle_options);
	listener.support(methods::GET, handle_get);
	listener.support(methods::POST, handle_post);

	try
	{
		listener.open().then([&listener]() {TRACE(L"\nLISTENING...\n"); }).wait();

		while (true);

	}
	catch (const exception &e)
	{
		cout << e.what() << endl;
	}

	getchar();

	return EXIT_SUCCESS;

}

