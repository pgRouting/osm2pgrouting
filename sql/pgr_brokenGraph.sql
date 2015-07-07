CREATE OR REPLACE FUNCTION pgr_brokenGraph(
                IN tbl character varying,
                IN clmn character varying
        )
        RETURNS SETOF VOID AS
$body$
DECLARE
        sql1 text;
        rec1 record;
        sql2 text;
        rec2 record;
        rec_count record;
        rec_single record;
        graph_id integer;
        gids int [];        
BEGIN   
        EXECUTE 'SELECT COUNT(*) AS count FROM information_schema.columns WHERE table_name= ''' || quote_ident(tbl) || ''' AND column_name= ''' || quote_ident(clmn) || '''' INTO rec_count;

        If (rec_count.count > 0) THEN
                RAISE NOTICE 'Column % in the table % already exists!', quote_ident(clmn), quote_ident(tbl);
        ELSE
                --------- Add necessary columns ----------
                EXECUTE 'ALTER TABLE ' || quote_ident(tbl) || ' ADD COLUMN ' || quote_ident(clmn) || ' INTEGER DEFAULT -1';
                EXECUTE 'ALTER TABLE ' || quote_ident(tbl) || ' ADD COLUMN garbage INTEGER DEFAULT 0';
                graph_id := 1;
                
                While true
                        Loop
                                ---------- Assign the very first -1 row graph_id ----------
                                EXECUTE 'SELECT * FROM ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = -1 LIMIT 1' INTO rec_single;
                                EXECUTE 'UPDATE ' || quote_ident(tbl) || ' SET ' || quote_ident(clmn) || ' = ' || graph_id || ' WHERE gid = ' || rec_single.gid || '';

                                --------- Search other rows with that particular graph_id -----------
                                while true
                                        loop
                                                execute 'SELECT COUNT(*) FROm ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = ' || graph_id || ' AND garbage = 0' into rec_count;
                                                ----------- The following if else will check those rows which already have entertained ------------
                                                if (rec_count.count > 0) then
                                                        sql1 := 'SELECT * FROm ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = ' || graph_id || ' AND garbage = 0';
                                                        for rec1 in execute sql1
                                                                loop
                                                                        sql2 := 'SELECT * FROM ' || quote_ident(tbl) || ' WHERE source = '|| rec1.source ||' OR target = '|| rec1.source ||' OR source = '|| rec1.target ||' OR target = '|| rec1.target ||'';
                                                                        for rec2 in execute sql2
                                                                                loop
                                                                                        EXECUTE 'UPDATE ' || quote_ident(tbl) || ' SET ' || quote_ident(clmn) || ' = ' || graph_id || ' WHERE gid = ' || rec2.gid || '';
                                                                                end loop;
                                                                        execute 'UPDATE ' || quote_ident(tbl) || ' SET garbage = 1 WHERE gid = ' || rec1.gid || '';
                                                                end loop;
                                                else
                                                        exit;
                                                end if;
                                        end loop;
                                
                                ------ Following is to exit the while loop. 0 means no more -1 id.
                                EXECUTE 'SELECT COUNT(*) AS count FROM ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = -1' INTO rec_count;
                                If (rec_count.count = 0) THEN
                                        EXIT;
                                ELSE
                                        graph_id := graph_id + 1;
                                END IF;
                        end loop;
                        
                ----------- Drop garbage column ------------
                EXECUTE 'ALTER TABLE ' || quote_ident(tbl) || ' DROP COLUMN garbage';
        END IF;
        
        EXCEPTION
                WHEN internal_error THEN
                        RAISE NOTICE 'Something went wrong!';
END;
$body$
LANGUAGE 'plpgsql' VOLATILE STRICT;

------------ Usage ------------
-- SELECT pgr_brokengraph('ways', 'graph_id')