#pragma once
#include <orm/db.hpp>
#include <tom/migration.hpp>

using Orm::DB;

namespace Migrations
{
    struct RecreateDatabaseStructure : Migration
    {
		T_MIGRATION

        void up() const override
        {
			Schema::create("posts", [](Blueprint &table)
            {
                table.id();

                table.string(NAME);
                table.timestamps();
            });
			Schema::create("groups", [](Blueprint &table)
			{
				table.id("id_group");
				table.unsignedBigInteger("id_group_1").nullable();
				table.string("name", 50);
				table.foreign("id_group_1").references("id_group").on("groups");
			});
			Schema::create("connection", [](Blueprint &table)
			{
				table.id("id_connection");
				table.string("name", 50);
			});
			Schema::create("localisation", [](Blueprint &table)
			{
				table.id("id_localisation");
				table.string("name", 50);
			});
			Schema::create("type", [](Blueprint &table)
			{
				table.id("id_type");
				table.string("name", 50);
			});
			Schema::create("content", [](Blueprint &table)
			{
				table.id("id_content");
				table.string("title", 50);
			});
			Schema::create("default_server_configuration_hallway", [](Blueprint &table)
			{
				table.id("id_dafault_hallway");
			});
			Schema::create("default_server_configuration_auditorium", [](Blueprint &table)
			{
				table.id("id_default_auditorium");
			});
			Schema::create("site", [](Blueprint &table)
			{
				table.id("id_site");
				table.unsignedBigInteger("id_group");
				table.unsignedBigInteger("id_connection");
				table.string("name", 50);
				table.foreign("id_group").references("id_group").on("groups");
				table.foreign("id_connection").references("id_connection").on("connection");
			});
			Schema::create("server_pair_configuration", [](Blueprint &table)
			{
				table.id("id_serv_pair_config");
				table.string("projection_server_ip", 50);
				table.string("auditorium_server_ip", 50);
				table.string("name", 50);
				table.unsignedBigInteger("id_default_auditorium");
				table.unsignedBigInteger("id_site");
				table.foreign("id_default_auditorium").references("id_default_auditorium").on("default_server_configuration_auditorium");
				table.foreign("id_site").references("id_site").on("site");
			});
			Schema::create("releases", [](Blueprint &table)
			{
				table.unsignedBigInteger("id_content");
				table.unsignedBigInteger("id_type");
				table.unsignedBigInteger("id_localisation");
				table.string("release_cis_path", 50).defaultValue("NULL").nullable();
				table.string("release_cpl_ref_path", 50);
				table.primary({"id_content", "id_type", "id_localisation"});
				table.foreign("id_content").references("id_content").on("content");
				table.foreign("id_type").references("id_type").on("type");
				table.foreign("id_localisation").references("id_localisation").on("localisation");
			});
			Schema::create("auditorium_par", [](Blueprint &table)
			{
				table.id("id_auditorium_par");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
			});
			Schema::create("auditorium_backlight", [](Blueprint &table)
			{
				table.id("id_auditorium_bl");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
			});
			Schema::create("auditorium_moving_head", [](Blueprint &table)
			{
				table.id("id_auditorium_mh");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
			});
			Schema::create("hallway_server_configuration", [](Blueprint &table)
			{
				table.unsignedBigInteger("id_serv_pair_config");
				table.unsignedBigInteger("id_dafault_hallway");
				table.primary("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
				table.foreign("id_dafault_hallway").references("id_dafault_hallway").on("default_server_configuration_hallway");
			});
			Schema::create("auditorium_panel", [](Blueprint &table)
			{
				table.id("id_auditorium_panel");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
			});
			Schema::create("hallway_par", [](Blueprint &table)
			{
				table.id("id_hallway_par");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("hallway_server_configuration");
			});
			Schema::create("hallway_panel", [](Blueprint &table)
			{
				table.id("id_hallway_panel");
				table.unsignedBigInteger("id_serv_pair_config");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("hallway_server_configuration");
			});
			Schema::table("cpl", [](Blueprint &table)
			{
				table.unsignedBigInteger("id_serv_pair_config");
				table.unsignedBigInteger("id_content");
				table.unsignedBigInteger("id_type");
				table.unsignedBigInteger("id_localisation");
				table.integer("type_cpl").nullable().change();
				table.integer("id").change();
				table.string("sha1_sync", 40).nullable().change();
                table.dropPrimary("id");
				table.primary({"id_serv_pair_config", "id_content", "id_type", "id_localisation"});
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
				table.foreign("id_content").references("id_content").on("releases");
				table.foreign("id_type").references("id_type").on("releases");
				table.foreign("id_localisation").references("id_localisation").on("releases");
			});
			DB::statement("ALTER TABLE cpl MODIFY id INT NULL ;");
			Schema::create("sync", [](Blueprint &table)
			{
				table.unsignedBigInteger("id_serv_pair_config");
				table.unsignedBigInteger("id_content");
				table.unsignedBigInteger("id_type");
				table.unsignedBigInteger("id_localisation");
				table.string("path_sync", 50);
				table.primary({"id_serv_pair_config", "id_content", "id_type", "id_localisation"});
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("server_pair_configuration");
				table.foreign("id_content").references("id_content").on("releases");
				table.foreign("id_type").references("id_type").on("releases");
				table.foreign("id_localisation").references("id_localisation").on("releases");
			});
			Schema::create("loopSync", [](Blueprint &table)
			{
				table.unsignedBigInteger("id_content");
				table.unsignedBigInteger("id_type");
				table.unsignedBigInteger("id_localisation");
				table.unsignedBigInteger("id_serv_pair_config");
				table.string("path_sync_loop", 50);
				table.primary({"id_serv_pair_config","id_content", "id_type", "id_localisation"});
				table.foreign("id_content").references("id_content").on("releases");
				table.foreign("id_type").references("id_type").on("releases");
				table.foreign("id_localisation").references("id_localisation").on("releases");
				table.foreign("id_serv_pair_config").references("id_serv_pair_config").on("hallway_server_configuration");
			});
            DB::statement("CREATE TRIGGER `cpl_exists` \
            BEFORE INSERT ON `sync` \
            FOR EACH ROW \
            BEGIN \
                IF NOT EXISTS (SELECT * FROM `cpl` WHERE id_serv_pair_config = NEW.id_serv_pair_config && id_type = NEW.id_type && id_content = NEW.id_content && id_localisation = NEW.id_localisation) THEN \
                    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Aucun CPL correspondant'; \
                END IF; \
            END;");
        }
        void down() const override
        {
            DB::statement("DROP TRIGGER `cpl_exists`");
            Schema::dropIfExists("LoopSync");
            Schema::dropIfExists("sync");
            Schema::table("cpl", [](Blueprint &table)
            {
                table.dropForeign("cpl_id_serv_pair_config_foreign");
                table.dropForeign("cpl_id_content_foreign");
                table.dropForeign("cpl_id_type_foreign");
                table.dropForeign("cpl_id_localisation_foreign");
                table.dropPrimary({"id_serv_pair_config", "id_content", "id_type", "id_localisation"});
                table.dropColumns({"id_serv_pair_config", "id_content", "id_type", "id_localisation"});
            });
			DB::statement("ALTER TABLE cpl AUTO_INCREMENT =4, MODIFY id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY");
            Schema::dropIfExists("hallway_panel");
            Schema::dropIfExists("hallway_par");
            Schema::dropIfExists("auditorium_panel");
            Schema::dropIfExists("hallway_server_configuration");
            Schema::dropIfExists("auditorium_moving_head");
            Schema::dropIfExists("auditorium_backlight");
            Schema::dropIfExists("auditorium_par");
            Schema::dropIfExists("releases");
            Schema::dropIfExists("server_pair_configuration");
            Schema::dropIfExists("site");
            Schema::dropIfExists("default_server_configuration_auditorium");
            Schema::dropIfExists("default_server_configuration_hallway");
            Schema::dropIfExists("content");
            Schema::dropIfExists("type");
            Schema::dropIfExists("localisation");
            Schema::dropIfExists("connection");
            Schema::dropIfExists("groups");
        }
    };
}