#include <iostream>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <string>
#include <stdio.h>
#include <map>
#include <exception>

using namespace std;

const char* PrinterName = "DYMO_LabelWriter_450";

class Error: public exception
{
public:
  Error(const string& Message): exception(), Message_(Message) {}
  virtual ~Error() throw() {}
  virtual const char* what() const throw() { return Message_.c_str(); }
private:
  string Message_;
};


map <string, string> gPaperNames;
typedef pair<string, string> str_pair;

void print_printers(){
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i --, dest ++)
    {
        if (dest->instance)
            printf("%s/%s\n", dest->name, dest->instance);
        else
            puts(dest->name);
    }
}

int main(int argc, char** argv)
{
  try
  {
    if (argc < 3)
      throw Error("Usage: PrintLabel <labelSize> <ImageName> [<ImageName> ...]");

    cout << "Printers:" << endl;
    print_printers();

    int             num_options = 0;
    cups_option_t*  options = NULL;

    std::string label_size = argv[1];

    if (label_size.compare("57x32mm") == 0) {
      num_options = cupsAddOption("PageSize", "w162h90", num_options, &options); //11354 Multi-Purpose
    } else if (label_size.compare("41x89mm") == 0) {
      num_options = cupsAddOption("PageSize", "w118h252", num_options, &options); //11356 White Name Badge
    } else if (label_size.compare("54x101mm") == 0) {
      num_options = cupsAddOption("PageSize", "w154h286.2", num_options, &options); //99014 Shipping
    } else {
      throw Error("Label size is not a valid size. Must be WxHmm. Check source code to find supported sizes");
    }

    //num_options = cupsAddOption("scaling", "100", num_options, &options);
    //num_options = cupsAddOption("orientation-requested", "4", num_options, &options);
    //num_options = cupsAddOption("DymoHalftoning", "ErrorDiffusion", num_options, &options);
    //num_options = cupsAddOption("DymoPrintQuality", "Graphics", num_options, &options);

    for (int i=2; i<argc; i++) {
      cupsPrintFile(PrinterName, argv[i], "Label", num_options, options);
    }
    cupsFreeOptions(num_options, options);

    return 0;
  }
  catch(std::exception& e)
  {
    fprintf(stderr, "%s", e.what());
    fprintf(stderr, "\n");
    return 1;
  }
}
