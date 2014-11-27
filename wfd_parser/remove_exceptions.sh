#!/bin/bash
sed -i '/YYCDEBUG << \"Reading a token: \";/{:a;N;/yyempty = false;/!ba;N;s/.*/YYCDEBUG << \"Reading a token: \";\nyyla\.type = yytranslate_ (yylex (\&yyla\.value));\nyyempty = false;\n}/};' $1
sed -i '/try/{:a;N;/YYCDEBUG << \"Starting parse\" << std::endl;/!ba;N;s/.*\n/{\nYYCDEBUG << \"Starting parse\" << std::endl;\n/};' $1
sed -i '/YY_REDUCE_PRINT (yyn);/{:a;N;/switch (yyn)/!ba;N;s/.*\n/YY_REDUCE_PRINT (yyn);\n{\nswitch (yyn)\n/};' $1
sed -i '/catch (const syntax_error\& yyexc)/{:a;N;/}/!ba;N;s/.*\n/\n/};' $1
sed -i '/catch (\.\.\.)/{:a;N;/throw;/!ba;N;s/.*\n/{\n/};' $1

