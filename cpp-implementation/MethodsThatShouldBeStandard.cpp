#include "MethodsThatShouldBeStandard.h"

void MethodsThatShouldBeStandard::StringExplode(string Str, string Delimiter, vector<string>* Results)
{
	int Found;
	Found = Str.find_first_of(Delimiter);

	while (Found != string::npos)
	{
		if (Found > 0)
		{
			Results->push_back(Str.substr(0, Found));
		}
		Str = Str.substr(Found + 1);
		Found = Str.find_first_of(Delimiter);
	}
	if (Str.length() > 0)
	{
		Results->push_back(Str);
	}
}
