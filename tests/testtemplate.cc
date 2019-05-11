#include "core/template.h"
#include "core/vfs.h"

#include "catch.hpp"

TEST_CASE("template-test_replace", "[template]") {
  Template t { "Hello {{@sender}}!" };
  REQUIRE_FALSE(t.GetErrors().HasErrors() );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  Defines defines;
  defines.Define("sender", "Buffy");
  REQUIRE(t.Evaluate(defines) == "Hello Buffy!");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_if", "[template]") {
  Template t { "{{ifdef sender}}Hello {{@sender}}!{{end}}" };
  REQUIRE_FALSE(t.GetErrors().HasErrors() );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  Defines defines_with_sender;
  defines_with_sender.Define("sender", "Buffy");
  Defines empty_define;

  REQUIRE(t.Evaluate(empty_define) == "");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  REQUIRE(t.Evaluate(defines_with_sender) == "Hello Buffy!");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_define", "[template]") {
  Template t { "{{set sender \"Buffy\"}}Hello {{@sender}}!" };
  REQUIRE_FALSE(t.GetErrors().HasErrors() );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  Defines defines;
  REQUIRE(t.Evaluate(defines) == "Hello Buffy!");
  REQUIRE_FALSE(defines.IsDefined("sender") );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_only_code", "[template]") {
  Template t { "{{set sender \"Buffy\" @sender}}" };
  REQUIRE_FALSE(t.GetErrors().HasErrors() );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  Defines defines;
  REQUIRE(t.Evaluate(defines) == "Buffy");
  REQUIRE_FALSE(defines.IsDefined("sender") );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_basic_filesystem", "[template]") {
  FileSystem filesys;
  auto catalog = FileSystemRootCatalog::AddRoot(&filesys);
  catalog->RegisterFileString("main", "main");

  Template t {&filesys, "main"};
  REQUIRE_FALSE(t.GetErrors().HasErrors() );
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");

  Defines defines;
  REQUIRE(t.Evaluate(defines) == "main");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_include_filesystem", "[template]") {
  FileSystem filesys;
  auto catalog = FileSystemRootCatalog::AddRoot(&filesys);
  catalog->RegisterFileString("main", "{{include \"included\"}}");
  catalog->RegisterFileString("included", "included");

  Template t {&filesys, "main"};
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
  REQUIRE_FALSE(t.GetErrors().HasErrors() );

  Defines defines;
  REQUIRE(t.Evaluate(defines) == "included");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}

TEST_CASE("template-test_scoping_filesystem", "[template]") {
  FileSystem filesys;
  auto catalog = FileSystemRootCatalog::AddRoot(&filesys);
  catalog->RegisterFileString("main", "{{include \"included\"}} {{@var}}!");
  catalog->RegisterFileString("included", "{{set var \"hello\" @var}}");

  Template t {&filesys, "main"};
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
  REQUIRE_FALSE(t.GetErrors().HasErrors() );

  Defines defines;
  defines.Define("var", "world");
  REQUIRE(t.Evaluate(defines) == "hello world!");
  REQUIRE(t.GetErrors().GetCombinedErrors() == "");
}
