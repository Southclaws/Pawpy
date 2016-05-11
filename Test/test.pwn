#include <a_samp>
#include <pawpy>


forward OnLocationFound(module[], result[], length);
forward stop();

new ret1 = true;

main()
{
//	new ret;
//	new a[4] = {16, 8, 4, 2};
//	new s[128] = {"a string variable being sent to a python script"};

	RunPythonThreaded("geoip", "lookup", "OnLocationFound", "s", "8.8.8.8");

	SetTimer("stop", 1000, false);
}

public OnLocationFound(module[], result[], length)
{
	printf("Module '%s' result: '%s', length: %d", module, result, length);

	if(!ret1)
		return 0;

	else
		return 1;
}

public stop()
{
	print("Stopping");
	ret1 = false;
}
