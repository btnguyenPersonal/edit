#include "testRender.h"
#include "../../src/util/defines.h"

struct testSuiteRun testRender() {
	std::vector<struct testRun> output = std::vector<struct testRun>();

	{
		output.push_back({"getColorFromChar 1", compare(getPrintColor(getColorFromChar('a')), getPrintColor(WHITE))});
		output.push_back({"getColorFromChar 2", compare(getPrintColor(getColorFromChar('[')), getPrintColor(GREEN))});
		output.push_back({"getColorFromChar 3", compare(getPrintColor(getColorFromChar(']')), getPrintColor(GREEN))});
		output.push_back({"getColorFromChar 4", compare(getPrintColor(getColorFromChar('{')), getPrintColor(MAGENTA))});
		output.push_back({"getColorFromChar 5", compare(getPrintColor(getColorFromChar('}')), getPrintColor(MAGENTA))});
		output.push_back({"getColorFromChar 6", compare(getPrintColor(getColorFromChar('(')), getPrintColor(YELLOW))});
		output.push_back({"getColorFromChar 7", compare(getPrintColor(getColorFromChar(')')), getPrintColor(YELLOW))});
		output.push_back({"getColorFromChar 8", compare(getPrintColor(getColorFromChar('\'')), getPrintColor(CYAN))});
		output.push_back({"getColorFromChar 9", compare(getPrintColor(getColorFromChar('"')), getPrintColor(CYAN))});
		output.push_back({"getColorFromChar 10", compare(getPrintColor(getColorFromChar('`')), getPrintColor(CYAN))});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, ' ', WHITE);
		output.push_back({"should insert pixels in the space case", compare(pixels, {{' ', WHITE}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, 'a', WHITE);
		output.push_back({"should insert pixels in the a case", compare(pixels, {{'a', WHITE}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '~', WHITE);
		output.push_back({"should insert pixels in the ~ case", compare(pixels, {{'~', WHITE}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '[', GREEN);
		output.push_back({"should insert pixels in the [ case of green", compare(pixels, {{'[', GREEN}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '(', invertColor(WHITE));
		output.push_back({"should insert pixels in the [ case of invertColor(WHITE)", compare(pixels, {{'(', invertColor(WHITE)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '', invertColor(WHITE));
		output.push_back({"should insert pixels in the non ascii case", compare(pixels, {{' ', invertColor(MAGENTA)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '	', invertColor(WHITE));
		output.push_back({"should insert pixels in the tab case when not in tab indent", compare(pixels, {{' ', invertColor(WHITE)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->options.indent_style = "tab";
		state->options.indent_size = 8;
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '	', invertColor(WHITE));
		output.push_back({"should insert pixels in the tab case when in tab indent", compare(pixels, {{' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->options.indent_style = "tab";
		state->options.indent_size = 4;
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, '	', invertColor(WHITE));
		output.push_back({"should insert pixels in the tab case when in tab size 4", compare(pixels, {{' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		state->options.indent_style = "tab";
		state->options.indent_size = 4;
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixel(state, &pixels, ' ', WHITE);
		insertPixel(state, &pixels, ' ', WHITE);
		insertPixel(state, &pixels, '	', invertColor(WHITE));
		output.push_back({"should insert pixels in the tab case when in tab size 4 and not on event tab placement", compare(pixels, {{' ', WHITE}, {' ', WHITE}, {' ', invertColor(WHITE)}, {' ', invertColor(WHITE)}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixels(state, &pixels, "abcdef", WHITE);
		output.push_back({"should insert pixels in the in the default case", compare(pixels, {{'a', WHITE}, {'b', WHITE}, {'c', WHITE}, {'d', WHITE}, {'e', WHITE}, {'f', WHITE}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixels(state, &pixels, "abcdef", WHITE);
		insertPixels(state, &pixels, "abcdef", RED);
		output.push_back({"should insert pixels in the in the right color", compare(pixels, {{'a', WHITE}, {'b', WHITE}, {'c', WHITE}, {'d', WHITE}, {'e', WHITE}, {'f', WHITE}, {'a', RED}, {'b', RED}, {'c', RED}, {'d', RED}, {'e', RED}, {'f', RED}})});
	}

	{
		State *state = new State("./test-file.h", {""});
		std::vector<Pixel> pixels = std::vector<Pixel>();
		insertPixels(state, &pixels, "abcdef", WHITE);
		insertPixels(state, &pixels, "abcdef", RED);
		renderPixels(state, 0, 0, pixels, false);
		output.push_back({"should render the inserted pixels to screenPixels at 0, 0",
				  compare(
				      getScreenPixels(),
				      {
					  {{'a', WHITE}, 0, 0},
					  {{'b', WHITE}, 0, 1},
					  {{'c', WHITE}, 0, 2},
					  {{'d', WHITE}, 0, 3},
					  {{'e', WHITE}, 0, 4},
					  {{'f', WHITE}, 0, 5},
					  {{'a', RED}, 0, 6},
					  {{'b', RED}, 0, 7},
					  {{'c', RED}, 0, 8},
					  {{'d', RED}, 0, 9},
					  {{'e', RED}, 0, 10},
					  {{'f', RED}, 0, 11},
				      })});
	}

	return {"test/util/testRender.cpp", output};
}
